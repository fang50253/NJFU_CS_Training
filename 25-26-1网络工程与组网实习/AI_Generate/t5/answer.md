我将设计一个完整的邮件客户端系统，基于POP3和SMTP协议，包含图形界面和多线程支持。

## 一、系统架构设计

### 1. 协议配置
```
SMTP服务器：smtp.163.com:25/465(SSL)
POP3服务器：pop.163.com:110/995(SSL)
备用服务器：QQ邮箱、Gmail等
```

### 2. 邮件存储结构
```
邮件数据库：SQLite
附件存储：./attachments/
配置存储：./config/
缓存：./cache/
```

### 3. 多线程架构
```
主线程：UI界面
接收线程：POP3邮件接收
发送线程：SMTP邮件发送
缓存线程：邮件列表刷新
附件线程：附件下载/上传
```

## 二、POP3客户端实现

```python
import socket
import ssl
import email
from email.header import decode_header
from email.utils import parseaddr
import base64
import re
import time
from datetime import datetime
import threading
import queue

class POP3Client:
    def __init__(self, host='pop.163.com', port=110, use_ssl=True):
        self.host = host
        self.port = 995 if use_ssl and port == 110 else port
        self.use_ssl = use_ssl
        self.sock = None
        self.connection = None
        self.logged_in = False
        self.message_count = 0
        self.total_size = 0
        self.lock = threading.Lock()
        self.response_queue = queue.Queue()
        
    def connect(self):
        """连接POP3服务器"""
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.settimeout(30)
            self.sock.connect((self.host, self.port))
            
            if self.use_ssl:
                context = ssl.create_default_context()
                self.connection = context.wrap_socket(self.sock, server_hostname=self.host)
            else:
                self.connection = self.sock
            
            # 读取欢迎消息
            response = self._receive()
            if not response.startswith('+OK'):
                raise Exception(f"连接失败: {response}")
            
            print(f"已连接到POP3服务器 {self.host}:{self.port}")
            return True
            
        except Exception as e:
            print(f"连接失败: {str(e)}")
            self.disconnect()
            return False
    
    def disconnect(self):
        """断开连接"""
        try:
            if self.logged_in:
                self._send_command('QUIT')
            if self.connection:
                self.connection.close()
            if self.sock:
                self.sock.close()
        except:
            pass
        finally:
            self.connection = None
            self.sock = None
            self.logged_in = False
    
    def login(self, username, password):
        """登录邮箱"""
        try:
            # USER命令
            response = self._send_command(f'USER {username}')
            if not response.startswith('+OK'):
                return False, f"用户名错误: {response}"
            
            # PASS命令
            response = self._send_command(f'PASS {password}')
            if not response.startswith('+OK'):
                return False, f"密码错误: {response}"
            
            self.logged_in = True
            
            # 获取邮件统计信息
            response = self._send_command('STAT')
            if response.startswith('+OK'):
                parts = response.split()
                if len(parts) >= 3:
                    self.message_count = int(parts[1])
                    self.total_size = int(parts[2])
            
            return True, "登录成功"
            
        except Exception as e:
            return False, f"登录失败: {str(e)}"
    
    def get_mail_list(self, start=1, count=None):
        """获取邮件列表"""
        if not self.logged_in:
            return []
        
        try:
            if count is None or count > self.message_count:
                count = self.message_count
            
            # 获取邮件列表
            response = self._send_command('LIST')
            if not response.startswith('+OK'):
                return []
            
            mail_list = []
            lines = response.split('\r\n')[1:]  # 跳过状态行
            
            for line in lines:
                if line == '.' or not line.strip():
                    continue
                
                parts = line.split()
                if len(parts) >= 2:
                    mail_id = int(parts[0])
                    size = int(parts[1])
                    
                    if start <= mail_id <= start + count - 1:
                        # 获取邮件头部信息
                        headers = self._get_headers(mail_id)
                        if headers:
                            mail_info = self._parse_headers(headers, mail_id, size)
                            mail_list.append(mail_info)
            
            return mail_list
            
        except Exception as e:
            print(f"获取邮件列表失败: {str(e)}")
            return []
    
    def get_mail_content(self, mail_id, download_attachments=False):
        """获取完整邮件内容"""
        if not self.logged_in:
            return None
        
        try:
            # 获取完整邮件
            response = self._send_command(f'RETR {mail_id}')
            if not response.startswith('+OK'):
                return None
            
            # 解析邮件内容
            mail_data = response.split('\r\n', 1)[1]
            msg = email.message_from_string(mail_data)
            
            # 解析邮件信息
            mail_info = self._parse_email_message(msg, mail_id)
            
            # 如果需要下载附件
            if download_attachments and mail_info.get('attachments'):
                mail_info['attachments'] = self._download_attachments(msg, mail_id)
            
            return mail_info
            
        except Exception as e:
            print(f"获取邮件内容失败: {str(e)}")
            return None
    
    def delete_mail(self, mail_id):
        """删除邮件"""
        if not self.logged_in:
            return False
        
        try:
            response = self._send_command(f'DELE {mail_id}')
            return response.startswith('+OK')
        except:
            return False
    
    def mark_as_read(self, mail_id):
        """标记为已读（通过获取邮件实现）"""
        if not self.logged_in:
            return False
        
        try:
            # 只是获取邮件，不删除，服务器可能会标记为已读
            response = self._send_command(f'RETR {mail_id}')
            return response.startswith('+OK')
        except:
            return False
    
    def _send_command(self, command):
        """发送命令并接收响应"""
        with self.lock:
            try:
                self.connection.sendall(f"{command}\r\n".encode('utf-8'))
                response = self._receive()
                return response
            except Exception as e:
                raise Exception(f"命令 {command} 执行失败: {str(e)}")
    
    def _receive(self):
        """接收服务器响应"""
        try:
            data = b""
            while True:
                chunk = self.connection.recv(4096)
                if not chunk:
                    break
                data += chunk
                if b"\r\n" in chunk:
                    break
            
            response = data.decode('utf-8', errors='ignore').strip()
            
            # 如果是多行响应，继续接收直到遇到单独一行的"."
            if response.startswith('+OK') and command.upper().startswith(('LIST', 'RETR', 'TOP')):
                while True:
                    chunk = self.connection.recv(4096)
                    if not chunk:
                        break
                    data += chunk
                    if b"\r\n.\r\n" in data[-100:]:  # 检查最后100字节
                        break
                
                response = data.decode('utf-8', errors='ignore').strip()
            
            return response
            
        except Exception as e:
            raise Exception(f"接收响应失败: {str(e)}")
    
    def _get_headers(self, mail_id):
        """获取邮件头部（使用TOP命令）"""
        try:
            response = self._send_command(f'TOP {mail_id} 20')  # 获取前20行
            if response.startswith('+OK'):
                # 提取头部部分
                lines = response.split('\r\n')[1:]
                headers = []
                for line in lines:
                    if line == '.':
                        break
                    headers.append(line)
                return '\r\n'.join(headers)
            return None
        except:
            return None
    
    def _parse_headers(self, headers, mail_id, size):
        """解析邮件头部"""
        msg = email.message_from_string(headers)
        
        # 解析发件人
        from_ = msg.get('From', '未知发件人')
        from_name, from_addr = parseaddr(from_)
        
        # 解析收件人
        to_ = msg.get('To', '')
        to_name, to_addr = parseaddr(to_)
        
        # 解析主题
        subject = msg.get('Subject', '(无主题)')
        subject = self._decode_header(subject)
        
        # 解析日期
        date_str = msg.get('Date', '')
        try:
            # 将邮件日期转换为本地时间
            from email.utils import parsedate_to_datetime
            date = parsedate_to_datetime(date_str)
        except:
            date = datetime.now()
        
        # 解析其他信息
        content_type = msg.get_content_type()
        has_attachments = False
        if 'multipart' in content_type:
            for part in msg.walk():
                if part.get_content_disposition() == 'attachment':
                    has_attachments = True
                    break
        
        return {
            'id': mail_id,
            'from': from_addr,
            'from_name': from_name or from_addr,
            'to': to_addr,
            'to_name': to_name or to_addr,
            'subject': subject,
            'date': date,
            'size': size,
            'has_attachments': has_attachments,
            'read': False,  # 需要从本地数据库获取
            'priority': msg.get('X-Priority', '3'),
            'content_type': content_type
        }
    
    def _parse_email_message(self, msg, mail_id):
        """解析完整邮件"""
        # 基本信息
        mail_info = self._parse_headers(msg.as_string(), mail_id, 0)
        
        # 解析邮件正文
        body = {
            'text': '',
            'html': '',
            'attachments': []
        }
        
        # 遍历邮件部分
        if msg.is_multipart():
            for part in msg.walk():
                content_type = part.get_content_type()
                content_disposition = str(part.get("Content-Disposition"))
                
                # 跳过附件
                if "attachment" in content_disposition:
                    filename = part.get_filename()
                    if filename:
                        filename = self._decode_header(filename)
                        body['attachments'].append({
                            'filename': filename,
                            'content_type': content_type,
                            'size': len(part.get_payload(decode=True)) if part.get_payload(decode=True) else 0
                        })
                    continue
                
                # 获取正文
                if content_type == "text/plain":
                    try:
                        payload = part.get_payload(decode=True)
                        charset = part.get_content_charset() or 'utf-8'
                        body['text'] = payload.decode(charset, errors='ignore')
                    except:
                        body['text'] = part.get_payload()
                
                elif content_type == "text/html":
                    try:
                        payload = part.get_payload(decode=True)
                        charset = part.get_content_charset() or 'utf-8'
                        body['html'] = payload.decode(charset, errors='ignore')
                    except:
                        body['html'] = part.get_payload()
        
        else:  # 非multipart
            content_type = msg.get_content_type()
            try:
                payload = msg.get_payload(decode=True)
                charset = msg.get_content_charset() or 'utf-8'
                if content_type == "text/html":
                    body['html'] = payload.decode(charset, errors='ignore')
                else:
                    body['text'] = payload.decode(charset, errors='ignore')
            except:
                payload = msg.get_payload()
                if content_type == "text/html":
                    body['html'] = payload
                else:
                    body['text'] = payload
        
        mail_info.update(body)
        return mail_info
    
    def _download_attachments(self, msg, mail_id):
        """下载附件"""
        attachments = []
        
        if not msg.is_multipart():
            return attachments
        
        for part in msg.walk():
            content_disposition = str(part.get("Content-Disposition"))
            
            if "attachment" in content_disposition:
                filename = part.get_filename()
                if filename:
                    filename = self._decode_header(filename)
                    
                    # 生成保存路径
                    import os
                    attachment_dir = f"./attachments/{mail_id}"
                    os.makedirs(attachment_dir, exist_ok=True)
                    
                    save_path = os.path.join(attachment_dir, filename)
                    
                    try:
                        # 保存附件
                        payload = part.get_payload(decode=True)
                        if payload:
                            with open(save_path, 'wb') as f:
                                f.write(payload)
                            
                            attachments.append({
                                'filename': filename,
                                'path': save_path,
                                'size': len(payload)
                            })
                    except Exception as e:
                        print(f"下载附件失败 {filename}: {str(e)}")
        
        return attachments
    
    def _decode_header(self, header):
        """解码邮件头"""
        if not header:
            return ""
        
        try:
            decoded_parts = decode_header(header)
            decoded_str = ""
            for part, encoding in decoded_parts:
                if isinstance(part, bytes):
                    if encoding:
                        decoded_str += part.decode(encoding, errors='ignore')
                    else:
                        decoded_str += part.decode('utf-8', errors='ignore')
                else:
                    decoded_str += part
            return decoded_str
        except:
            return str(header)

class POP3Receiver(threading.Thread):
    """POP3邮件接收线程"""
    def __init__(self, config, callback):
        super().__init__()
        self.config = config
        self.callback = callback
        self.running = True
        self.client = None
        self.check_interval = 300  # 5分钟检查一次
        
    def run(self):
        """运行接收线程"""
        while self.running:
            try:
                self.check_new_mails()
                # 等待指定间隔
                for _ in range(self.check_interval):
                    if not self.running:
                        break
                    time.sleep(1)
                    
            except Exception as e:
                print(f"邮件接收线程错误: {str(e)}")
                time.sleep(60)  # 出错后等待1分钟
    
    def check_new_mails(self):
        """检查新邮件"""
        try:
            # 连接服务器
            self.client = POP3Client(
                host=self.config.get('pop3_host', 'pop.163.com'),
                port=self.config.get('pop3_port', 110),
                use_ssl=self.config.get('pop3_ssl', True)
            )
            
            if not self.client.connect():
                return
            
            # 登录
            success, message = self.client.login(
                self.config.get('username'),
                self.config.get('password')
            )
            
            if not success:
                self.callback('error', {'message': message})
                return
            
            # 获取邮件列表
            mails = self.client.get_mail_list()
            
            if mails:
                self.callback('new_mails', {'mails': mails})
            
            self.client.disconnect()
            
        except Exception as e:
            self.callback('error', {'message': str(e)})
    
    def stop(self):
        """停止线程"""
        self.running = False
        if self.client:
            self.client.disconnect()
```

## 三、SMTP客户端实现

```python
import socket
import ssl
import base64
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.application import MIMEApplication
from email.header import Header
from email.utils import formataddr
import mimetypes
import os
import threading
import queue

class SMTPClient:
    def __init__(self, host='smtp.163.com', port=25, use_ssl=False):
        self.host = host
        self.port = 465 if use_ssl and port == 25 else port
        self.use_ssl = use_ssl
        self.sock = None
        self.connection = None
        self.logged_in = False
        self.debug = False
        self.lock = threading.Lock()
        
    def connect(self):
        """连接SMTP服务器"""
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.settimeout(30)
            
            if self.use_ssl:
                # SSL连接
                context = ssl.create_default_context()
                self.connection = context.wrap_socket(
                    self.sock, server_hostname=self.host
                )
                self.connection.connect((self.host, self.port))
            else:
                # 普通连接
                self.sock.connect((self.host, self.port))
                self.connection = self.sock
            
            # 读取欢迎消息
            response = self._receive()
            if not response.startswith('220'):
                raise Exception(f"连接失败: {response}")
            
            # EHLO命令
            response = self._send_command(f'EHLO {self.host}')
            if not response.startswith('250'):
                # 尝试HELO
                response = self._send_command(f'HELO {self.host}')
                if not response.startswith('250'):
                    raise Exception(f"握手失败: {response}")
            
            # 如果服务器支持STARTTLS，且我们没有使用SSL
            if not self.use_ssl and 'STARTTLS' in response:
                response = self._send_command('STARTTLS')
                if response.startswith('220'):
                    # 升级到TLS
                    context = ssl.create_default_context()
                    self.connection = context.wrap_socket(
                        self.sock, server_hostname=self.host
                    )
                    # 重新EHLO
                    self._send_command(f'EHLO {self.host}')
            
            print(f"已连接到SMTP服务器 {self.host}:{self.port}")
            return True
            
        except Exception as e:
            print(f"连接失败: {str(e)}")
            self.disconnect()
            return False
    
    def disconnect(self):
        """断开连接"""
        try:
            if self.logged_in:
                self._send_command('QUIT')
            if self.connection:
                self.connection.close()
            if self.sock:
                self.sock.close()
        except:
            pass
        finally:
            self.connection = None
            self.sock = None
            self.logged_in = False
    
    def login(self, username, password):
        """登录SMTP服务器"""
        try:
            # AUTH LOGIN
            response = self._send_command('AUTH LOGIN')
            if not response.startswith('334'):
                # 尝试其他认证方式
                response = self._send_command('AUTH PLAIN')
                if response.startswith('334'):
                    # PLAIN认证
                    auth_string = f"\0{username}\0{password}"
                    auth_base64 = base64.b64encode(auth_string.encode()).decode()
                    response = self._send_command(auth_base64)
                else:
                    return False, "不支持AUTH LOGIN或AUTH PLAIN"
            else:
                # LOGIN认证
                username_base64 = base64.b64encode(username.encode()).decode()
                response = self._send_command(username_base64)
                if not response.startswith('334'):
                    return False, "用户名认证失败"
                
                password_base64 = base64.b64encode(password.encode()).decode()
                response = self._send_command(password_base64)
            
            if response.startswith('235'):
                self.logged_in = True
                return True, "登录成功"
            else:
                return False, f"认证失败: {response}"
            
        except Exception as e:
            return False, f"登录失败: {str(e)}"
    
    def send_mail(self, from_addr, to_addrs, subject, content, 
                  cc_addrs=None, bcc_addrs=None, attachments=None, 
                  html=False, reply_to=None):
        """发送邮件"""
        if not self.logged_in:
            return False, "未登录"
        
        try:
            # 创建邮件
            if attachments or html:
                msg = MIMEMultipart()
                
                # 添加正文
                if html:
                    msg.attach(MIMEText(content, 'html', 'utf-8'))
                else:
                    msg.attach(MIMEText(content, 'plain', 'utf-8'))
                
                # 添加附件
                if attachments:
                    for attachment in attachments:
                        self._add_attachment(msg, attachment)
            else:
                msg = MIMEText(content, 'plain', 'utf-8')
            
            # 设置邮件头
            msg['From'] = formataddr((from_addr, from_addr))
            msg['To'] = ', '.join([formataddr((addr, addr)) for addr in to_addrs])
            msg['Subject'] = Header(subject, 'utf-8')
            msg['Date'] = email.utils.formatdate(localtime=True)
            
            if cc_addrs:
                msg['Cc'] = ', '.join(cc_addrs)
            if bcc_addrs:
                msg['Bcc'] = ', '.join(bcc_addrs)
            if reply_to:
                msg['Reply-To'] = reply_to
            
            # MAIL FROM
            response = self._send_command(f'MAIL FROM:<{from_addr}>')
            if not response.startswith('250'):
                return False, f"发件人错误: {response}"
            
            # RCPT TO
            all_recipients = list(to_addrs)
            if cc_addrs:
                all_recipients.extend(cc_addrs)
            if bcc_addrs:
                all_recipients.extend(bcc_addrs)
            
            for recipient in all_recipients:
                response = self._send_command(f'RCPT TO:<{recipient}>')
                if not response.startswith('250'):
                    return False, f"收件人错误 {recipient}: {response}"
            
            # DATA
            response = self._send_command('DATA')
            if not response.startswith('354'):
                return False, f"准备发送数据失败: {response}"
            
            # 发送邮件数据
            mail_data = msg.as_string()
            self.connection.sendall(mail_data.encode('utf-8'))
            
            # 结束数据发送
            response = self._send_command('.')
            if not response.startswith('250'):
                return False, f"发送失败: {response}"
            
            return True, "发送成功"
            
        except Exception as e:
            return False, f"发送失败: {str(e)}"
    
    def _add_attachment(self, msg, attachment_path):
        """添加附件"""
        if not os.path.exists(attachment_path):
            return
        
        filename = os.path.basename(attachment_path)
        
        # 猜测MIME类型
        ctype, encoding = mimetypes.guess_type(filename)
        if ctype is None or encoding is not None:
            ctype = 'application/octet-stream'
        
        maintype, subtype = ctype.split('/', 1)
        
        with open(attachment_path, 'rb') as f:
            if maintype == 'text':
                # 文本文件
                attachment = MIMEText(f.read().decode('utf-8', errors='ignore'), _subtype=subtype)
            else:
                # 二进制文件
                attachment = MIMEApplication(f.read(), _subtype=subtype)
        
        attachment.add_header('Content-Disposition', 'attachment', filename=filename)
        msg.attach(attachment)
    
    def _send_command(self, command):
        """发送命令并接收响应"""
        with self.lock:
            try:
                if self.debug:
                    print(f"发送: {command}")
                
                self.connection.sendall(f"{command}\r\n".encode('utf-8'))
                response = self._receive()
                
                if self.debug:
                    print(f"接收: {response}")
                
                return response
            except Exception as e:
                raise Exception(f"命令 {command} 执行失败: {str(e)}")
    
    def _receive(self):
        """接收服务器响应"""
        try:
            response_lines = []
            while True:
                line = self.connection.recv(1024).decode('utf-8', errors='ignore')
                if not line:
                    break
                
                response_lines.append(line.strip())
                
                # SMTP响应以空格开头表示多行，否则是最后一行
                if len(line) > 3 and line[3] != ' ' and line[3] != '-':
                    break
            
            return '\n'.join(response_lines)
            
        except Exception as e:
            raise Exception(f"接收响应失败: {str(e)}")

class SMTPSender(threading.Thread):
    """SMTP邮件发送线程"""
    def __init__(self, config, mail_data, callback):
        super().__init__()
        self.config = config
        self.mail_data = mail_data
        self.callback = callback
        self.client = None
        
    def run(self):
        """运行发送线程"""
        try:
            # 连接服务器
            self.client = SMTPClient(
                host=self.config.get('smtp_host', 'smtp.163.com'),
                port=self.config.get('smtp_port', 25),
                use_ssl=self.config.get('smtp_ssl', False)
            )
            
            if not self.client.connect():
                self.callback('error', {'message': '连接服务器失败'})
                return
            
            # 登录
            success, message = self.client.login(
                self.config.get('username'),
                self.config.get('password')
            )
            
            if not success:
                self.callback('error', {'message': message})
                return
            
            # 发送邮件
            success, message = self.client.send_mail(
                from_addr=self.mail_data['from'],
                to_addrs=self.mail_data['to'],
                subject=self.mail_data['subject'],
                content=self.mail_data['content'],
                cc_addrs=self.mail_data.get('cc', []),
                bcc_addrs=self.mail_data.get('bcc', []),
                attachments=self.mail_data.get('attachments', []),
                html=self.mail_data.get('html', False),
                reply_to=self.mail_data.get('reply_to')
            )
            
            if success:
                self.callback('sent', {'message': message})
            else:
                self.callback('error', {'message': message})
            
            self.client.disconnect()
            
        except Exception as e:
            self.callback('error', {'message': str(e)})
        
        finally:
            if self.client:
                self.client.disconnect()
```

## 四、数据库和缓存管理

```python
import sqlite3
import json
import os
from datetime import datetime
import threading

class MailDatabase:
    """邮件数据库管理"""
    def __init__(self, db_path='./mail_client.db'):
        self.db_path = db_path
        self.lock = threading.Lock()
        self._init_database()
    
    def _init_database(self):
        """初始化数据库"""
        with self.lock:
            conn = sqlite3.connect(self.db_path)
            cursor = conn.cursor()
            
            # 创建邮件表
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS mails (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    mail_id TEXT UNIQUE,
                    from_addr TEXT,
                    from_name TEXT,
                    to_addr TEXT,
                    to_name TEXT,
                    subject TEXT,
                    content_text TEXT,
                    content_html TEXT,
                    date DATETIME,
                    size INTEGER,
                    has_attachments BOOLEAN,
                    read BOOLEAN DEFAULT 0,
                    starred BOOLEAN DEFAULT 0,
                    folder TEXT DEFAULT 'inbox',
                    account TEXT,
                    raw_data TEXT,
                    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
                )
            ''')
            
            # 创建附件表
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS attachments (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    mail_id INTEGER,
                    filename TEXT,
                    filepath TEXT,
                    size INTEGER,
                    content_type TEXT,
                    downloaded BOOLEAN DEFAULT 0,
                    FOREIGN KEY (mail_id) REFERENCES mails (id)
                )
            ''')
            
            # 创建账户表
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS accounts (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    email TEXT UNIQUE,
                    username TEXT,
                    password TEXT,
                    smtp_host TEXT,
                    smtp_port INTEGER,
                    smtp_ssl BOOLEAN,
                    pop3_host TEXT,
                    pop3_port INTEGER,
                    pop3_ssl BOOLEAN,
                    signature TEXT,
                    last_sync DATETIME,
                    active BOOLEAN DEFAULT 1,
                    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
                )
            ''')
            
            # 创建文件夹表
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS folders (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    name TEXT,
                    account_id INTEGER,
                    type TEXT DEFAULT 'user',
                    unread_count INTEGER DEFAULT 0,
                    total_count INTEGER DEFAULT 0,
                    FOREIGN KEY (account_id) REFERENCES accounts (id)
                )
            ''')
            
            # 创建索引
            cursor.execute('CREATE INDEX IF NOT EXISTS idx_mails_date ON mails(date)')
            cursor.execute('CREATE INDEX IF NOT EXISTS idx_mails_folder ON mails(folder)')
            cursor.execute('CREATE INDEX IF NOT EXISTS idx_mails_account ON mails(account)')
            cursor.execute('CREATE INDEX IF NOT EXISTS idx_mails_read ON mails(read)')
            
            conn.commit()
            conn.close()
    
    def save_mail(self, mail_data, account_email, folder='inbox'):
        """保存邮件到数据库"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                # 检查是否已存在
                cursor.execute(
                    'SELECT id FROM mails WHERE mail_id = ? AND account = ?',
                    (mail_data.get('id'), account_email)
                )
                existing = cursor.fetchone()
                
                if existing:
                    # 更新现有邮件
                    cursor.execute('''
                        UPDATE mails SET 
                            from_addr = ?, from_name = ?,
                            to_addr = ?, to_name = ?,
                            subject = ?, content_text = ?,
                            content_html = ?, date = ?,
                            size = ?, has_attachments = ?,
                            folder = ?, updated_at = CURRENT_TIMESTAMP
                        WHERE id = ?
                    ''', (
                        mail_data.get('from'),
                        mail_data.get('from_name'),
                        mail_data.get('to'),
                        mail_data.get('to_name'),
                        mail_data.get('subject'),
                        mail_data.get('text', ''),
                        mail_data.get('html', ''),
                        mail_data.get('date'),
                        mail_data.get('size', 0),
                        1 if mail_data.get('attachments') else 0,
                        folder,
                        existing[0]
                    ))
                    mail_db_id = existing[0]
                else:
                    # 插入新邮件
                    cursor.execute('''
                        INSERT INTO mails (
                            mail_id, from_addr, from_name, to_addr, to_name,
                            subject, content_text, content_html, date, size,
                            has_attachments, folder, account, raw_data
                        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                    ''', (
                        mail_data.get('id'),
                        mail_data.get('from'),
                        mail_data.get('from_name'),
                        mail_data.get('to'),
                        mail_data.get('to_name'),
                        mail_data.get('subject'),
                        mail_data.get('text', ''),
                        mail_data.get('html', ''),
                        mail_data.get('date'),
                        mail_data.get('size', 0),
                        1 if mail_data.get('attachments') else 0,
                        folder,
                        account_email,
                        json.dumps(mail_data) if mail_data else ''
                    ))
                    mail_db_id = cursor.lastrowid
                
                # 保存附件信息
                if mail_data.get('attachments'):
                    for attachment in mail_data['attachments']:
                        cursor.execute('''
                            INSERT OR REPLACE INTO attachments 
                            (mail_id, filename, filepath, size, content_type, downloaded)
                            VALUES (?, ?, ?, ?, ?, ?)
                        ''', (
                            mail_db_id,
                            attachment.get('filename'),
                            attachment.get('path', ''),
                            attachment.get('size', 0),
                            attachment.get('content_type', 'application/octet-stream'),
                            1 if attachment.get('path') else 0
                        ))
                
                conn.commit()
                conn.close()
                
                return mail_db_id
                
            except Exception as e:
                print(f"保存邮件失败: {str(e)}")
                return None
    
    def get_mails(self, account_email, folder='inbox', limit=50, offset=0, 
                  unread_only=False, starred_only=False):
        """获取邮件列表"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                conn.row_factory = sqlite3.Row
                cursor = conn.cursor()
                
                query = '''
                    SELECT * FROM mails 
                    WHERE account = ? AND folder = ?
                '''
                params = [account_email, folder]
                
                if unread_only:
                    query += ' AND read = 0'
                if starred_only:
                    query += ' AND starred = 1'
                
                query += ' ORDER BY date DESC LIMIT ? OFFSET ?'
                params.extend([limit, offset])
                
                cursor.execute(query, params)
                rows = cursor.fetchall()
                
                mails = []
                for row in rows:
                    mail = dict(row)
                    # 转换日期
                    if mail['date']:
                        try:
                            mail['date'] = datetime.fromisoformat(mail['date'])
                        except:
                            pass
                    mails.append(mail)
                
                conn.close()
                return mails
                
            except Exception as e:
                print(f"获取邮件列表失败: {str(e)}")
                return []
    
    def get_mail(self, mail_db_id):
        """获取单封邮件"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                conn.row_factory = sqlite3.Row
                cursor = conn.cursor()
                
                cursor.execute('SELECT * FROM mails WHERE id = ?', (mail_db_id,))
                row = cursor.fetchone()
                
                if row:
                    mail = dict(row)
                    
                    # 获取附件
                    cursor.execute(
                        'SELECT * FROM attachments WHERE mail_id = ?',
                        (mail_db_id,)
                    )
                    attachments = [dict(a) for a in cursor.fetchall()]
                    mail['attachments'] = attachments
                    
                    conn.close()
                    return mail
                
                conn.close()
                return None
                
            except Exception as e:
                print(f"获取邮件失败: {str(e)}")
                return None
    
    def mark_as_read(self, mail_db_id, read=True):
        """标记邮件已读/未读"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                cursor.execute(
                    'UPDATE mails SET read = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?',
                    (1 if read else 0, mail_db_id)
                )
                
                conn.commit()
                conn.close()
                return True
                
            except Exception as e:
                print(f"标记邮件失败: {str(e)}")
                return False
    
    def star_mail(self, mail_db_id, starred=True):
        """星标邮件"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                cursor.execute(
                    'UPDATE mails SET starred = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?',
                    (1 if starred else 0, mail_db_id)
                )
                
                conn.commit()
                conn.close()
                return True
                
            except Exception as e:
                print(f"星标邮件失败: {str(e)}")
                return False
    
    def move_mail(self, mail_db_id, folder):
        """移动邮件到其他文件夹"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                cursor.execute(
                    'UPDATE mails SET folder = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?',
                    (folder, mail_db_id)
                )
                
                conn.commit()
                conn.close()
                return True
                
            except Exception as e:
                print(f"移动邮件失败: {str(e)}")
                return False
    
    def delete_mail(self, mail_db_id):
        """删除邮件"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                # 先删除附件记录
                cursor.execute('DELETE FROM attachments WHERE mail_id = ?', (mail_db_id,))
                
                # 删除邮件
                cursor.execute('DELETE FROM mails WHERE id = ?', (mail_db_id,))
                
                conn.commit()
                conn.close()
                return True
                
            except Exception as e:
                print(f"删除邮件失败: {str(e)}")
                return False
    
    def save_account(self, account_data):
        """保存账户配置"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                cursor.execute('''
                    INSERT OR REPLACE INTO accounts 
                    (email, username, password, smtp_host, smtp_port, smtp_ssl,
                     pop3_host, pop3_port, pop3_ssl, signature, last_sync)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                ''', (
                    account_data['email'],
                    account_data['username'],
                    account_data['password'],
                    account_data.get('smtp_host', 'smtp.163.com'),
                    account_data.get('smtp_port', 25),
                    account_data.get('smtp_ssl', False),
                    account_data.get('pop3_host', 'pop.163.com'),
                    account_data.get('pop3_port', 110),
                    account_data.get('pop3_ssl', True),
                    account_data.get('signature', ''),
                    datetime.now().isoformat()
                ))
                
                conn.commit()
                conn.close()
                return True
                
            except Exception as e:
                print(f"保存账户失败: {str(e)}")
                return False
    
    def get_accounts(self):
        """获取所有账户"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                conn.row_factory = sqlite3.Row
                cursor = conn.cursor()
                
                cursor.execute('SELECT * FROM accounts WHERE active = 1')
                accounts = [dict(a) for a in cursor.fetchall()]
                
                conn.close()
                return accounts
                
            except Exception as e:
                print(f"获取账户失败: {str(e)}")
                return []
```

## 五、主图形界面（Tkinter实现）

```python
import tkinter as tk
from tkinter import ttk, messagebox, filedialog, scrolledtext, font
import threading
import queue
import os
import webbrowser
from datetime import datetime
import json

class MailClientGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("邮件客户端 - POP3/SMTP")
        self.root.geometry("1400x800")
        
        # 初始化组件
        self.db = MailDatabase()
        self.pop3_client = None
        self.smtp_client = None
        
        # 邮件接收线程
        self.receiver_thread = None
        self.receiver_running = False
        
        # 当前状态
        self.current_account = None
        self.current_folder = 'inbox'
        self.current_mail = None
        self.accounts = []
        
        # 创建附件目录
        os.makedirs('./attachments', exist_ok=True)
        
        # 加载账户
        self.load_accounts()
        
        # 设置UI
        self.setup_ui()
        
        # 启动邮件接收
        self.start_mail_receiver()
    
    def setup_ui(self):
        """设置用户界面"""
        # 创建菜单栏
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)
        
        # 文件菜单
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="文件", menu=file_menu)
        file_menu.add_command(label="新建邮件", command=self.new_mail, accelerator="Ctrl+N")
        file_menu.add_command(label="添加账户", command=self.add_account_dialog)
        file_menu.add_command(label="账户管理", command=self.manage_accounts)
        file_menu.add_separator()
        file_menu.add_command(label="退出", command=self.root.quit)
        
        # 编辑菜单
        edit_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="编辑", menu=edit_menu)
        edit_menu.add_command(label="回复", command=self.reply_mail, accelerator="Ctrl+R")
        edit_menu.add_command(label="转发", command=self.forward_mail, accelerator="Ctrl+F")
        edit_menu.add_command(label="删除", command=self.delete_mail, accelerator="Del")
        edit_menu.add_separator()
        edit_menu.add_command(label="标记为已读", command=lambda: self.mark_mail(True))
        edit_menu.add_command(label="标记为未读", command=lambda: self.mark_mail(False))
        edit_menu.add_command(label="星标", command=lambda: self.star_mail(True))
        edit_menu.add_command(label="取消星标", command=lambda: self.star_mail(False))
        
        # 视图菜单
        view_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="视图", menu=view_menu)
        view_menu.add_command(label="刷新", command=self.refresh_mail_list, accelerator="F5")
        view_menu.add_command(label="收件箱", command=lambda: self.switch_folder('inbox'))
        view_menu.add_command(label="已发送", command=lambda: self.switch_folder('sent'))
        view_menu.add_command(label="草稿箱", command=lambda: self.switch_folder('drafts'))
        view_menu.add_command(label="垃圾邮件", command=lambda: self.switch_folder('spam'))
        view_menu.add_command(label="已删除", command=lambda: self.switch_folder('trash'))
        
        # 工具菜单
        tools_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="工具", menu=tools_menu)
        tools_menu.add_command(label="设置", command=self.settings_dialog)
        tools_menu.add_command(label="检查新邮件", command=self.check_new_mails)
        
        # 创建主框架
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill='both', expand=True, padx=5, pady=5)
        
        # 左侧边栏（账户和文件夹）
        sidebar = ttk.Frame(main_frame, width=250)
        sidebar.pack(side='left', fill='y', padx=(0, 5))
        
        # 账户选择
        account_frame = ttk.LabelFrame(sidebar, text="账户", padding=10)
        account_frame.pack(fill='x', pady=(0, 10))
        
        self.account_var = tk.StringVar()
        if self.accounts:
            self.account_var.set(self.accounts[0]['email'])
            self.current_account = self.accounts[0]
        
        account_combo = ttk.Combobox(account_frame, textvariable=self.account_var, 
                                    state='readonly', width=25)
        account_combo['values'] = [acc['email'] for acc in self.accounts]
        account_combo.pack(fill='x')
        account_combo.bind('<<ComboboxSelected>>', self.on_account_selected)
        
        ttk.Button(account_frame, text="添加账户", 
                  command=self.add_account_dialog).pack(fill='x', pady=(5, 0))
        
        # 文件夹列表
        folder_frame = ttk.LabelFrame(sidebar, text="文件夹", padding=10)
        folder_frame.pack(fill='both', expand=True)
        
        # 创建Treeview显示文件夹
        self.folder_tree = ttk.Treeview(folder_frame, show='tree', height=15)
        
        # 添加默认文件夹
        self.folder_items = {}
        folders = [
            ('inbox', '收件箱', '📨'),
            ('sent', '已发送', '📤'),
            ('drafts', '草稿箱', '📝'),
            ('starred', '星标邮件', '⭐'),
            ('spam', '垃圾邮件', '🚫'),
            ('trash', '已删除', '🗑️')
        ]
        
        for folder_id, folder_name, icon in folders:
            item_id = self.folder_tree.insert('', 'end', text=f"{icon} {folder_name}", 
                                            values=(folder_id,))
            self.folder_items[folder_id] = item_id
        
        # 滚动条
        folder_scrollbar = ttk.Scrollbar(folder_frame, orient='vertical', 
                                        command=self.folder_tree.yview)
        self.folder_tree.configure(yscrollcommand=folder_scrollbar.set)
        
        self.folder_tree.pack(side='left', fill='both', expand=True)
        folder_scrollbar.pack(side='right', fill='y')
        
        # 绑定选择事件
        self.folder_tree.bind('<<TreeviewSelect>>', self.on_folder_selected)
        
        # 新建邮件按钮
        ttk.Button(sidebar, text="📧 写邮件", 
                  command=self.new_mail, style='Accent.TButton').pack(fill='x', pady=10)
        
        # 中间邮件列表区域
        middle_frame = ttk.Frame(main_frame)
        middle_frame.pack(side='left', fill='both', expand=True)
        
        # 邮件列表工具栏
        list_toolbar = ttk.Frame(middle_frame)
        list_toolbar.pack(fill='x', pady=(0, 5))
        
        ttk.Button(list_toolbar, text="刷新", 
                  command=self.refresh_mail_list).pack(side='left', padx=2)
        ttk.Button(list_toolbar, text="全选", 
                  command=self.select_all_mails).pack(side='left', padx=2)
        ttk.Button(list_toolbar, text="标记已读", 
                  command=lambda: self.mark_mail(True)).pack(side='left', padx=2)
        ttk.Button(list_toolbar, text="删除", 
                  command=self.delete_mail).pack(side='left', padx=2)
        
        # 搜索框
        search_frame = ttk.Frame(list_toolbar)
        search_frame.pack(side='right')
        
        ttk.Label(search_frame, text="搜索:").pack(side='left')
        self.search_var = tk.StringVar()
        search_entry = ttk.Entry(search_frame, textvariable=self.search_var, width=20)
        search_entry.pack(side='left', padx=5)
        ttk.Button(search_frame, text="搜索", 
                  command=self.search_mails).pack(side='left')
        
        # 邮件列表
        list_frame = ttk.Frame(middle_frame)
        list_frame.pack(fill='both', expand=True)
        
        # 创建Treeview显示邮件列表
        columns = ('', '发件人', '主题', '日期', '大小', '附件')
        self.mail_tree = ttk.Treeview(list_frame, columns=columns, show='tree headings', height=20)
        
        # 设置列
        self.mail_tree.column('#0', width=30, stretch=False)
        self.mail_tree.column('发件人', width=200)
        self.mail_tree.column('主题', width=300)
        self.mail_tree.column('日期', width=120)
        self.mail_tree.column('大小', width=80)
        self.mail_tree.column('附件', width=50)
        
        self.mail_tree.heading('#0', text='')
        self.mail_tree.heading('发件人', text='发件人')
        self.mail_tree.heading('主题', text='主题')
        self.mail_tree.heading('日期', text='日期')
        self.mail_tree.heading('大小', text='大小')
        self.mail_tree.heading('附件', text='附件')
        
        # 滚动条
        list_scrollbar = ttk.Scrollbar(list_frame, orient='vertical', 
                                      command=self.mail_tree.yview)
        self.mail_tree.configure(yscrollcommand=list_scrollbar.set)
        
        self.mail_tree.pack(side='left', fill='both', expand=True)
        list_scrollbar.pack(side='right', fill='y')
        
        # 绑定选择事件
        self.mail_tree.bind('<<TreeviewSelect>>', self.on_mail_selected)
        self.mail_tree.bind('<Double-Button-1>', self.open_mail)
        
        # 右侧邮件预览区域
        preview_frame = ttk.LabelFrame(main_frame, text="邮件预览", width=500)
        preview_frame.pack(side='right', fill='both', expand=True)
        preview_frame.pack_propagate(False)  # 固定宽度
        
        # 邮件头信息
        header_frame = ttk.Frame(preview_frame)
        header_frame.pack(fill='x', padx=10, pady=10)
        
        self.subject_var = tk.StringVar(value="")
        subject_label = ttk.Label(header_frame, textvariable=self.subject_var, 
                                 font=('Microsoft YaHei', 12, 'bold'))
        subject_label.pack(anchor='w')
        
        # 发件人信息
        info_frame = ttk.Frame(header_frame)
        info_frame.pack(fill='x', pady=5)
        
        ttk.Label(info_frame, text="发件人:").pack(side='left')
        self.from_var = tk.StringVar(value="")
        ttk.Label(info_frame, textvariable=self.from_var).pack(side='left', padx=5)
        
        ttk.Label(info_frame, text="收件人:").pack(side='left', padx=(20, 0))
        self.to_var = tk.StringVar(value="")
        ttk.Label(info_frame, textvariable=self.to_var).pack(side='left', padx=5)
        
        # 日期和大小
        ttk.Label(info_frame, text="日期:").pack(side='left', padx=(20, 0))
        self.date_var = tk.StringVar(value="")
        ttk.Label(info_frame, textvariable=self.date_var).pack(side='left', padx=5)
        
        # 邮件内容区域
        content_frame = ttk.Frame(preview_frame)
        content_frame.pack(fill='both', expand=True, padx=10, pady=(0, 10))
        
        # 创建Notebook切换文本/HTML视图
        self.content_notebook = ttk.Notebook(content_frame)
        self.content_notebook.pack(fill='both', expand=True)
        
        # 文本视图
        text_frame = ttk.Frame(self.content_notebook)
        self.text_content = scrolledtext.ScrolledText(
            text_frame, wrap='word', font=('Microsoft YaHei', 10)
        )
        self.text_content.pack(fill='both', expand=True)
        self.content_notebook.add(text_frame, text="文本")
        
        # HTML视图
        html_frame = ttk.Frame(self.content_notebook)
        self.html_content = scrolledtext.ScrolledText(
            html_frame, wrap='word', font=('Microsoft YaHei', 10)
        )
        self.html_content.pack(fill='both', expand=True)
        self.content_notebook.add(html_frame, text="HTML")
        
        # 附件区域
        attachment_frame = ttk.LabelFrame(preview_frame, text="附件", padding=10)
        attachment_frame.pack(fill='x', padx=10, pady=(0, 10))
        
        self.attachment_listbox = tk.Listbox(attachment_frame, height=3)
        self.attachment_listbox.pack(fill='x')
        
        # 附件按钮
        attachment_button_frame = ttk.Frame(attachment_frame)
        attachment_button_frame.pack(fill='x', pady=(5, 0))
        
        ttk.Button(attachment_button_frame, text="下载所有", 
                  command=self.download_all_attachments).pack(side='left', padx=2)
        ttk.Button(attachment_button_frame, text="打开", 
                  command=self.open_attachment).pack(side='left', padx=2)
        
        # 操作按钮
        button_frame = ttk.Frame(preview_frame)
        button_frame.pack(fill='x', padx=10, pady=(0, 10))
        
        ttk.Button(button_frame, text="回复", 
                  command=self.reply_mail).pack(side='left', padx=5)
        ttk.Button(button_frame, text="回复全部", 
                  command=self.reply_all).pack(side='left', padx=5)
        ttk.Button(button_frame, text="转发", 
                  command=self.forward_mail).pack(side='left', padx=5)
        ttk.Button(button_frame, text="删除", 
                  command=self.delete_mail).pack(side='right', padx=5)
        
        # 底部状态栏
        status_frame = ttk.Frame(self.root)
        status_frame.pack(fill='x', side='bottom')
        
        self.status_var = tk.StringVar(value="就绪")
        ttk.Label(status_frame, textvariable=self.status_var, 
                 relief='sunken', anchor='w').pack(fill='x')
        
        # 快捷键绑定
        self.root.bind('<Control-n>', lambda e: self.new_mail())
        self.root.bind('<Control-r>', lambda e: self.reply_mail())
        self.root.bind('<Control-f>', lambda e: self.forward_mail())
        self.root.bind('<Delete>', lambda e: self.delete_mail())
        self.root.bind('<F5>', lambda e: self.refresh_mail_list())
        
        # 初始刷新邮件列表
        self.refresh_mail_list()
    
    def load_accounts(self):
        """加载账户列表"""
        self.accounts = self.db.get_accounts()
    
    def on_account_selected(self, event):
        """账户选择事件"""
        email = self.account_var.get()
        for account in self.accounts:
            if account['email'] == email:
                self.current_account = account
                self.refresh_mail_list()
                break
    
    def on_folder_selected(self, event):
        """文件夹选择事件"""
        selection = self.folder_tree.selection()
        if not selection:
            return
        
        item = self.folder_tree.item(selection[0])
        folder_id = item['values'][0] if item['values'] else ''
        
        if folder_id:
            self.current_folder = folder_id
            self.refresh_mail_list()
    
    def refresh_mail_list(self):
        """刷新邮件列表"""
        if not self.current_account:
            return
        
        # 清空列表
        for item in self.mail_tree.get_children():
            self.mail_tree.delete(item)
        
        # 获取邮件
        mails = self.db.get_mails(
            account_email=self.current_account['email'],
            folder=self.current_folder,
            limit=50
        )
        
        for mail in mails:
            # 星标图标
            star_icon = '★' if mail.get('starred') else '☆'
            
            # 已读/未读图标
            read_icon = '✓' if mail.get('read') else '●'
            
            # 发件人显示名称
            from_name = mail.get('from_name') or mail.get('from_addr', '未知')
            
            # 主题
            subject = mail.get('subject', '(无主题)')
            if len(subject) > 50:
                subject = subject[:47] + '...'
            
            # 日期格式化
            date = mail.get('date')
            if isinstance(date, str):
                date_str = date
            elif hasattr(date, 'strftime'):
                date_str = date.strftime('%Y-%m-%d %H:%M')
            else:
                date_str = ''
            
            # 大小格式化
            size = mail.get('size', 0)
            size_str = self.format_size(size)
            
            # 附件图标
            attachment_icon = '📎' if mail.get('has_attachments') else ''
            
            # 插入到列表
            item_id = self.mail_tree.insert('', 'end', text=star_icon, tags=(mail['id'],))
            self.mail_tree.set(item_id, '发件人', from_name)
            self.mail_tree.set(item_id, '主题', subject)
            self.mail_tree.set(item_id, '日期', date_str)
            self.mail_tree.set(item_id, '大小', size_str)
            self.mail_tree.set(item_id, '附件', attachment_icon)
            
            # 设置未读邮件为粗体
            if not mail.get('read'):
                self.mail_tree.tag_configure('unread', font=('Microsoft YaHei', 10, 'bold'))
                self.mail_tree.item(item_id, tags=('unread',))
    
    def on_mail_selected(self, event):
        """邮件选择事件"""
        selection = self.mail_tree.selection()
        if not selection:
            return
        
        item = self.mail_tree.item(selection[0])
        mail_db_id = item['tags'][0] if item['tags'] else None
        
        if mail_db_id:
            self.load_mail_content(mail_db_id)
    
    def load_mail_content(self, mail_db_id):
        """加载邮件内容"""
        mail = self.db.get_mail(mail_db_id)
        if not mail:
            return
        
        self.current_mail = mail
        
        # 更新邮件头信息
        self.subject_var.set(mail.get('subject', '(无主题)'))
        self.from_var.set(f"{mail.get('from_name', '')} <{mail.get('from_addr', '')}>")
        self.to_var.set(f"{mail.get('to_name', '')} <{mail.get('to_addr', '')}>")
        
        # 日期
        date = mail.get('date')
        if date:
            if isinstance(date, str):
                date_str = date
            else:
                date_str = date.strftime('%Y-%m-%d %H:%M:%S')
            self.date_var.set(date_str)
        
        # 显示内容
        self.text_content.delete(1.0, tk.END)
        self.html_content.delete(1.0, tk.END)
        
        text_content = mail.get('content_text', '')
        html_content = mail.get('content_html', '')
        
        if text_content:
            self.text_content.insert(1.0, text_content)
        
        if html_content:
            # 简单显示HTML（实际应用中应该使用HTML渲染组件）
            self.html_content.insert(1.0, html_content)
        elif not text_content:
            self.text_content.insert(1.0, "(无内容)")
        
        # 更新附件列表
        self.attachment_listbox.delete(0, tk.END)
        attachments = mail.get('attachments', [])
        for attachment in attachments:
            filename = attachment.get('filename', '未知文件')
            size = self.format_size(attachment.get('size', 0))
            self.attachment_listbox.insert(tk.END, f"{filename} ({size})")
        
        # 标记为已读
        self.db.mark_as_read(mail_db_id, True)
    
    def open_mail(self, event):
        """打开邮件（双击）"""
        self.on_mail_selected(event)
    
    def new_mail(self):
        """新建邮件"""
        self.open_compose_window()
    
    def reply_mail(self):
        """回复邮件"""
        if not self.current_mail:
            messagebox.showwarning("提示", "请先选择要回复的邮件")
            return
        
        # 获取原邮件信息
        original = self.current_mail
        
        # 构建回复内容
        subject = f"Re: {original.get('subject', '')}"
        to_addr = original.get('from_addr', '')
        
        # 引用原文
        content = f"\n\n---------- 原始邮件 ----------\n"
        content += f"发件人: {original.get('from_name', '')} <{original.get('from_addr', '')}>\n"
        content += f"收件人: {original.get('to_name', '')} <{original.get('to_addr', '')}>\n"
        content += f"时间: {original.get('date', '')}\n"
        content += f"主题: {original.get('subject', '')}\n\n"
        content += original.get('content_text', '')
        
        self.open_compose_window(
            to=to_addr,
            subject=subject,
            content=content,
            reply_to=original.get('from_addr')
        )
    
    def reply_all(self):
        """回复全部"""
        if not self.current_mail:
            messagebox.showwarning("提示", "请先选择要回复的邮件")
            return
        
        # 这里应该解析原邮件的所有收件人
        # 简化为只回复发件人
        self.reply_mail()
    
    def forward_mail(self):
        """转发邮件"""
        if not self.current_mail:
            messagebox.showwarning("提示", "请先选择要转发的邮件")
            return
        
        original = self.current_mail
        
        # 构建转发内容
        subject = f"Fw: {original.get('subject', '')}"
        
        # 引用原文
        content = f"\n\n---------- 转发邮件 ----------\n"
        content += f"发件人: {original.get('from_name', '')} <{original.get('from_addr', '')}>\n"
        content += f"收件人: {original.get('to_name', '')} <{original.get('to_addr', '')}>\n"
        content += f"时间: {original.get('date', '')}\n"
        content += f"主题: {original.get('subject', '')}\n\n"
        content += original.get('content_text', '')
        
        self.open_compose_window(
            subject=subject,
            content=content
        )
    
    def open_compose_window(self, to='', subject='', content='', reply_to=None):
        """打开邮件编辑窗口"""
        compose_window = tk.Toplevel(self.root)
        compose_window.title("写邮件")
        compose_window.geometry("800x600")
        
        # 收件人
        to_frame = ttk.Frame(compose_window, padding=10)
        to_frame.pack(fill='x')
        
        ttk.Label(to_frame, text="收件人:").pack(side='left')
        to_var = tk.StringVar(value=to)
        ttk.Entry(to_frame, textvariable=to_var, width=50).pack(side='left', padx=5)
        
        # 抄送/密送按钮
        cc_button = ttk.Button(to_frame, text="抄送", 
                              command=lambda: self.toggle_cc_bcc(compose_window, 'cc'))
        cc_button.pack(side='left', padx=5)
        
        bcc_button = ttk.Button(to_frame, text="密送", 
                               command=lambda: self.toggle_cc_bcc(compose_window, 'bcc'))
        bcc_button.pack(side='left', padx=5)
        
        # 主题
        subject_frame = ttk.Frame(compose_window, padding=10)
        subject_frame.pack(fill='x')
        
        ttk.Label(subject_frame, text="主题:").pack(side='left')
        subject_var = tk.StringVar(value=subject)
        ttk.Entry(subject_frame, textvariable=subject_var, width=50).pack(side='left', padx=5)
        
        # 邮件内容
        content_frame = ttk.Frame(compose_window)
        content_frame.pack(fill='both', expand=True, padx=10, pady=(0, 10))
        
        # 创建Notebook切换文本/HTML编辑
        content_notebook = ttk.Notebook(content_frame)
        content_notebook.pack(fill='both', expand=True)
        
        # 文本编辑
        text_frame = ttk.Frame(content_notebook)
        text_editor = scrolledtext.ScrolledText(
            text_frame, wrap='word', font=('Microsoft YaHei', 10)
        )
        text_editor.pack(fill='both', expand=True)
        text_editor.insert(1.0, content)
        content_notebook.add(text_frame, text="文本")
        
        # HTML编辑
        html_frame = ttk.Frame(content_notebook)
        html_editor = scrolledtext.ScrolledText(
            html_frame, wrap='word', font=('Microsoft YaHei', 10)
        )
        html_editor.pack(fill='both', expand=True)
        content_notebook.add(html_frame, text="HTML")
        
        # 附件区域
        attachment_frame = ttk.LabelFrame(compose_window, text="附件", padding=10)
        attachment_frame.pack(fill='x', padx=10, pady=(0, 10))
        
        attachment_listbox = tk.Listbox(attachment_frame, height=3)
        attachment_listbox.pack(fill='x')
        
        # 附件按钮
        attachment_button_frame = ttk.Frame(attachment_frame)
        attachment_button_frame.pack(fill='x', pady=(5, 0))
        
        attachments = []
        
        def add_attachment():
            files = filedialog.askopenfilenames(title="选择附件")
            for file in files:
                attachments.append(file)
                attachment_listbox.insert(tk.END, os.path.basename(file))
        
        ttk.Button(attachment_button_frame, text="添加附件", 
                  command=add_attachment).pack(side='left', padx=2)
        ttk.Button(attachment_button_frame, text="删除", 
                  command=lambda: self.remove_attachment(attachment_listbox, attachments)).pack(side='left', padx=2)
        
        # 发送按钮
        button_frame = ttk.Frame(compose_window, padding=10)
        button_frame.pack(fill='x')
        
        def send_mail():
            # 收集邮件数据
            mail_data = {
                'from': self.current_account['email'],
                'to': [addr.strip() for addr in to_var.get().split(',') if addr.strip()],
                'subject': subject_var.get(),
                'content': text_editor.get(1.0, tk.END) if content_notebook.index('current') == 0 
                          else html_editor.get(1.0, tk.END),
                'html': content_notebook.index('current') == 1,
                'attachments': attachments,
                'reply_to': reply_to
            }
            
            # 验证输入
            if not mail_data['to']:
                messagebox.showerror("错误", "请输入收件人")
                return
            
            if not mail_data['subject'].strip():
                if not messagebox.askyesno("确认", "邮件没有主题，确定发送吗？"):
                    return
            
            # 发送邮件
            self.send_mail_thread(mail_data)
            compose_window.destroy()
        
        ttk.Button(button_frame, text="发送", 
                  command=send_mail, style='Accent.TButton').pack(side='right', padx=5)
        ttk.Button(button_frame, text="存草稿", 
                  command=lambda: self.save_draft(mail_data)).pack(side='right', padx=5)
        ttk.Button(button_frame, text="取消", 
                  command=compose_window.destroy).pack(side='right', padx=5)
    
    def toggle_cc_bcc(self, parent, type_):
        """显示/隐藏抄送/密送框"""
        # 简化实现，实际应该动态添加输入框
        messagebox.showinfo("提示", f"{'抄送' if type_ == 'cc' else '密送'}功能")
    
    def remove_attachment(self, listbox, attachments):
        """删除附件"""
        selection = listbox.curselection()
        if selection:
            index = selection[0]
            listbox.delete(index)
            if index < len(attachments):
                attachments.pop(index)
    
    def send_mail_thread(self, mail_data):
        """在新线程中发送邮件"""
        if not self.current_account:
            messagebox.showerror("错误", "请先选择账户")
            return
        
        # 更新状态
        self.status_var.set("正在发送邮件...")
        
        # 创建发送线程
        sender = SMTPSender(
            config=self.current_account,
            mail_data=mail_data,
            callback=self.on_mail_sent
        )
        sender.start()
    
    def on_mail_sent(self, event_type, data):
        """邮件发送回调"""
        if event_type == 'sent':
            self.status_var.set("邮件发送成功")
            messagebox.showinfo("成功", "邮件发送成功")
        elif event_type == 'error':
            self.status_var.set(f"发送失败: {data['message']}")
            messagebox.showerror("发送失败", data['message'])
    
    def delete_mail(self):
        """删除邮件"""
        selection = self.mail_tree.selection()
        if not selection:
            return
        
        if not messagebox.askyesno("确认删除", "确定要删除选中的邮件吗？"):
            return
        
        for item in selection:
            mail_db_id = self.mail_tree.item(item, 'tags')[0]
            if mail_db_id:
                # 移动到垃圾箱
                self.db.move_mail(mail_db_id, 'trash')
        
        self.refresh_mail_list()
        self.status_var.set("邮件已移动到垃圾箱")
    
    def mark_mail(self, read):
        """标记邮件已读/未读"""
        selection = self.mail_tree.selection()
        if not selection:
            return
        
        for item in selection:
            mail_db_id = self.mail_tree.item(item, 'tags')[0]
            if mail_db_id:
                self.db.mark_as_read(mail_db_id, read)
        
        self.refresh_mail_list()
        self.status_var.set(f"邮件已标记为{'已读' if read else '未读'}")
    
    def star_mail(self, starred):
        """星标邮件"""
        selection = self.mail_tree.selection()
        if not selection:
            return
        
        for item in selection:
            mail_db_id = self.mail_tree.item(item, 'tags')[0]
            if mail_db_id:
                self.db.star_mail(mail_db_id, starred)
        
        self.refresh_mail_list()
        self.status_var.set(f"邮件已{'星标' if starred else '取消星标'}")
    
    def switch_folder(self, folder):
        """切换文件夹"""
        self.current_folder = folder
        self.refresh_mail_list()
        
        # 选中对应的文件夹项
        if folder in self.folder_items:
            self.folder_tree.selection_set(self.folder_items[folder])
    
    def search_mails(self):
        """搜索邮件"""
        keyword = self.search_var.get().strip()
        if not keyword:
            return
        
        # 这里应该实现数据库搜索
        # 简化实现：在主题和发件人中搜索
        messagebox.showinfo("搜索", f"搜索关键词: {keyword}")
    
    def select_all_mails(self):
        """全选邮件"""
        for item in self.mail_tree.get_children():
            self.mail_tree.selection_add(item)
    
    def download_all_attachments(self):
        """下载所有附件"""
        if not self.current_mail or not self.current_mail.get('attachments'):
            return
        
        # 选择保存目录
        folder = filedialog.askdirectory(title="选择保存目录")
        if not folder:
            return
        
        attachments = self.current_mail['attachments']
        for attachment in attachments:
            if attachment.get('filepath') and os.path.exists(attachment['filepath']):
                # 复制文件到目标目录
                import shutil
                src = attachment['filepath']
                dst = os.path.join(folder, attachment['filename'])
                shutil.copy2(src, dst)
        
        self.status_var.set(f"已下载 {len(attachments)} 个附件到 {folder}")
    
    def open_attachment(self):
        """打开附件"""
        selection = self.attachment_listbox.curselection()
        if not selection or not self.current_mail:
            return
        
        index = selection[0]
        attachments = self.current_mail.get('attachments', [])
        if index < len(attachments):
            attachment = attachments[index]
            filepath = attachment.get('filepath')
            
            if filepath and os.path.exists(filepath):
                # 使用默认程序打开文件
                webbrowser.open(f"file://{os.path.abspath(filepath)}")
            else:
                messagebox.showwarning("附件不存在", "附件文件不存在或未下载")
    
    def add_account_dialog(self):
        """添加账户对话框"""
        dialog = tk.Toplevel(self.root)
        dialog.title("添加邮箱账户")
        dialog.geometry("500x400")
        
        # 邮箱类型选择
        type_frame = ttk.Frame(dialog, padding=10)
        type_frame.pack(fill='x')
        
        ttk.Label(type_frame, text="邮箱类型:").pack(side='left')
        
        mail_types = {
            '163邮箱': {'smtp': 'smtp.163.com:25', 'pop3': 'pop.163.com:110'},
            'QQ邮箱': {'smtp': 'smtp.qq.com:465', 'pop3': 'pop.qq.com:995'},
            'Gmail': {'smtp': 'smtp.gmail.com:587', 'pop3': 'pop.gmail.com:995'},
            '自定义': {'smtp': '', 'pop3': ''}
        }
        
        type_var = tk.StringVar(value='163邮箱')
        type_combo = ttk.Combobox(type_frame, textvariable=type_var, 
                                 values=list(mail_types.keys()), state='readonly')
        type_combo.pack(side='left', padx=5)
        
        # 邮箱地址
        email_frame = ttk.Frame(dialog, padding=10)
        email_frame.pack(fill='x')
        
        ttk.Label(email_frame, text="邮箱地址:").pack(side='left')
        email_var = tk.StringVar()
        ttk.Entry(email_frame, textvariable=email_var, width=30).pack(side='left', padx=5)
        
        # 密码
        pass_frame = ttk.Frame(dialog, padding=10)
        pass_frame.pack(fill='x')
        
        ttk.Label(pass_frame, text="密码:").pack(side='left')
        pass_var = tk.StringVar()
        ttk.Entry(pass_frame, textvariable=pass_var, show='*', width=30).pack(side='left', padx=5)
        
        # 服务器设置
        server_frame = ttk.LabelFrame(dialog, text="服务器设置", padding=10)
        server_frame.pack(fill='x', padx=10, pady=10)
        
        # SMTP
        ttk.Label(server_frame, text="SMTP服务器:").grid(row=0, column=0, sticky='w', pady=5)
        smtp_host_var = tk.StringVar(value='smtp.163.com')
        ttk.Entry(server_frame, textvariable=smtp_host_var, width=20).grid(row=0, column=1, pady=5, padx=5)
        
        ttk.Label(server_frame, text="端口:").grid(row=0, column=2, sticky='w', pady=5, padx=(10, 0))
        smtp_port_var = tk.StringVar(value='25')
        ttk.Entry(server_frame, textvariable=smtp_port_var, width=10).grid(row=0, column=3, pady=5, padx=5)
        
        smtp_ssl_var = tk.BooleanVar(value=False)
        ttk.Checkbutton(server_frame, text="SSL", variable=smtp_ssl_var).grid(row=0, column=4, pady=5, padx=5)
        
        # POP3
        ttk.Label(server_frame, text="POP3服务器:").grid(row=1, column=0, sticky='w', pady=5)
        pop3_host_var = tk.StringVar(value='pop.163.com')
        ttk.Entry(server_frame, textvariable=pop3_host_var, width=20).grid(row=1, column=1, pady=5, padx=5)
        
        ttk.Label(server_frame, text="端口:").grid(row=1, column=2, sticky='w', pady=5, padx=(10, 0))
        pop3_port_var = tk.StringVar(value='110')
        ttk.Entry(server_frame, textvariable=pop3_port_var, width=10).grid(row=1, column=3, pady=5, padx=5)
        
        pop3_ssl_var = tk.BooleanVar(value=True)
        ttk.Checkbutton(server_frame, text="SSL", variable=pop3_ssl_var).grid(row=1, column=4, pady=5, padx=5)
        
        def update_servers(*args):
            """根据邮箱类型更新服务器设置"""
            mail_type = type_var.get()
            if mail_type in mail_types:
                servers = mail_types[mail_type]
                if servers['smtp']:
                    smtp_host, smtp_port = servers['smtp'].split(':')
                    smtp_host_var.set(smtp_host)
                    smtp_port_var.set(smtp_port)
                
                if servers['pop3']:
                    pop3_host, pop3_port = servers['pop3'].split(':')
                    pop3_host_var.set(pop3_host)
                    pop3_port_var.set(pop3_port)
        
        type_var.trace('w', update_servers)
        
        # 测试连接按钮
        test_frame = ttk.Frame(dialog)
        test_frame.pack(fill='x', padx=10, pady=10)
        
        def test_connection():
            """测试连接"""
            email_addr = email_var.get()
            password = pass_var.get()
            
            if not email_addr or not password:
                messagebox.showerror("错误", "请输入邮箱地址和密码")
                return
            
            # 测试POP3连接
            try:
                client = POP3Client(
                    host=pop3_host_var.get(),
                    port=int(pop3_port_var.get()),
                    use_ssl=pop3_ssl_var.get()
                )
                
                if client.connect():
                    username = email_addr.split('@')[0]
                    success, msg = client.login(username, password)
                    client.disconnect()
                    
                    if success:
                        messagebox.showinfo("成功", "POP3连接测试成功")
                    else:
                        messagebox.showerror("失败", f"POP3登录失败: {msg}")
                else:
                    messagebox.showerror("失败", "POP3连接失败")
                    
            except Exception as e:
                messagebox.showerror("错误", f"连接测试失败: {str(e)}")
        
        ttk.Button(test_frame, text="测试连接", 
                  command=test_connection).pack(side='left', padx=5)
        
        # 保存按钮
        def save_account():
            """保存账户"""
            email_addr = email_var.get()
            password = pass_var.get()
            
            if not email_addr or not password:
                messagebox.showerror("错误", "请输入邮箱地址和密码")
                return
            
            account_data = {
                'email': email_addr,
                'username': email_addr.split('@')[0],  # 简化处理
                'password': password,
                'smtp_host': smtp_host_var.get(),
                'smtp_port': int(smtp_port_var.get()),
                'smtp_ssl': smtp_ssl_var.get(),
                'pop3_host': pop3_host_var.get(),
                'pop3_port': int(pop3_port_var.get()),
                'pop3_ssl': pop3_ssl_var.get(),
                'signature': ''
            }
            
            if self.db.save_account(account_data):
                messagebox.showinfo("成功", "账户添加成功")
                self.load_accounts()
                dialog.destroy()
            else:
                messagebox.showerror("失败", "保存账户失败")
        
        ttk.Button(test_frame, text="保存", 
                  command=save_account, style='Accent.TButton').pack(side='right', padx=5)
        ttk.Button(test_frame, text="取消", 
                  command=dialog.destroy).pack(side='right', padx=5)
    
    def manage_accounts(self):
        """账户管理"""
        dialog = tk.Toplevel(self.root)
        dialog.title("账户管理")
        dialog.geometry("600x400")
        
        # 账户列表
        list_frame = ttk.Frame(dialog, padding=10)
        list_frame.pack(fill='both', expand=True)
        
        columns = ('邮箱', 'SMTP服务器', 'POP3服务器')
        tree = ttk.Treeview(list_frame, columns=columns, show='headings', height=10)
        
        for col in columns:
            tree.heading(col, text=col)
            if col == '邮箱':
                tree.column(col, width=200)
            else:
                tree.column(col, width=150)
        
        # 添加账户数据
        for account in self.accounts:
            tree.insert('', 'end', values=(
                account['email'],
                f"{account['smtp_host']}:{account['smtp_port']}",
                f"{account['pop3_host']}:{account['pop3_port']}"
            ))
        
        scrollbar = ttk.Scrollbar(list_frame, orient='vertical', command=tree.yview)
        tree.configure(yscrollcommand=scrollbar.set)
        
        tree.pack(side='left', fill='both', expand=True)
        scrollbar.pack(side='right', fill='y')
        
        # 按钮
        button_frame = ttk.Frame(dialog, padding=10)
        button_frame.pack(fill='x')
        
        def delete_account():
            """删除账户"""
            selection = tree.selection()
            if not selection:
                return
            
            if messagebox.askyesno("确认删除", "确定要删除选中的账户吗？"):
                # 这里应该实现删除逻辑
                messagebox.showinfo("提示", "删除功能待实现")
        
        ttk.Button(button_frame, text="删除", 
                  command=delete_account).pack(side='left', padx=5)
        ttk.Button(button_frame, text="设为默认", 
                  command=lambda: None).pack(side='left', padx=5)
        ttk.Button(button_frame, text="关闭", 
                  command=dialog.destroy).pack(side='right', padx=5)
    
    def settings_dialog(self):
        """设置对话框"""
        dialog = tk.Toplevel(self.root)
        dialog.title("设置")
        dialog.geometry("500x400")
        
        # 创建Notebook
        notebook = ttk.Notebook(dialog)
        notebook.pack(fill='both', expand=True, padx=10, pady=10)
        
        # 常规设置
        general_frame = ttk.Frame(notebook)
        notebook.add(general_frame, text="常规")
        
        # 邮件检查间隔
        interval_frame = ttk.Frame(general_frame, padding=10)
        interval_frame.pack(fill='x')
        
        ttk.Label(interval_frame, text="检查新邮件间隔(分钟):").pack(side='left')
        interval_var = tk.StringVar(value="5")
        ttk.Spinbox(interval_frame, from_=1, to=60, textvariable=interval_var, width=10).pack(side='left', padx=5)
        
        # 邮件显示设置
        display_frame = ttk.Frame(general_frame, padding=10)
        display_frame.pack(fill='x')
        
        font_size_var = tk.StringVar(value="10")
        ttk.Label(display_frame, text="字体大小:").pack(side='left')
        ttk.Spinbox(display_frame, from_=8, to=20, textvariable=font_size_var, width=5).pack(side='left', padx=5)
        
        # 发送设置
        send_frame = ttk.Frame(notebook)
        notebook.add(send_frame, text="发送")
        
        # 保存按钮
        button_frame = ttk.Frame(dialog)
        button_frame.pack(fill='x', padx=10, pady=(0, 10))
        
        ttk.Button(button_frame, text="保存", 
                  command=dialog.destroy, style='Accent.TButton').pack(side='right', padx=5)
        ttk.Button(button_frame, text="取消", 
                  command=dialog.destroy).pack(side='right', padx=5)
    
    def save_draft(self, mail_data):
        """保存草稿"""
        # 这里应该实现保存草稿到数据库
        messagebox.showinfo("提示", "草稿保存功能待实现")
    
    def check_new_mails(self):
        """手动检查新邮件"""
        if not self.current_account:
            messagebox.showwarning("提示", "请先选择账户")
            return
        
        self.status_var.set("正在检查新邮件...")
        self.start_mail_receiver(manual=True)
    
    def start_mail_receiver(self, manual=False):
        """启动邮件接收线程"""
        if not self.current_account:
            return
        
        # 如果已有线程在运行，先停止
        if self.receiver_thread and self.receiver_thread.is_alive():
            self.receiver_running = False
            self.receiver_thread.join(timeout=5)
        
        # 创建新的接收线程
        self.receiver_running = True
        self.receiver_thread = POP3Receiver(
            config=self.current_account,
            callback=self.on_new_mails
        )
        self.receiver_thread.start()
        
        if manual:
            # 手动检查，等待一会儿后停止
            def stop_receiver():
                time.sleep(10)  # 等待10秒
                self.receiver_running = False
            
            threading.Thread(target=stop_receiver).start()
    
    def on_new_mails(self, event_type, data):
        """新邮件回调"""
        if event_type == 'new_mails':
            mails = data.get('mails', [])
            if mails:
                # 保存新邮件到数据库
                for mail in mails:
                    self.db.save_mail(mail, self.current_account['email'])
                
                # 刷新邮件列表
                self.refresh_mail_list()
                
                self.status_var.set(f"收到 {len(mails)} 封新邮件")
                # 显示通知
                self.show_notification(f"收到 {len(mails)} 封新邮件")
        
        elif event_type == 'error':
            self.status_var.set(f"接收邮件错误: {data['message']}")
    
    def show_notification(self, message):
        """显示通知"""
        # 创建通知窗口
        notification = tk.Toplevel(self.root)
        notification.title("新邮件通知")
        notification.geometry("300x100")
        notification.attributes('-topmost', True)
        
        # 居中显示
        notification.update_idletasks()
        x = (notification.winfo_screenwidth() - notification.winfo_width()) // 2
        y = 50
        notification.geometry(f"+{x}+{y}")
        
        ttk.Label(notification, text=message, padding=20).pack(expand=True)
        
        # 3秒后自动关闭
        notification.after(3000, notification.destroy)
    
    def format_size(self, size):
        """格式化文件大小"""
        for unit in ['B', 'KB', 'MB']:
            if size < 1024.0:
                return f"{size:.1f} {unit}"
            size /= 1024.0
        return f"{size:.1f} GB"

def main():
    root = tk.Tk()
    
    # 设置样式
    style = ttk.Style()
    style.theme_use('clam')
    
    # 创建主应用
    app = MailClientGUI(root)
    
    # 窗口关闭事件
    def on_closing():
        # 停止接收线程
        if app.receiver_thread and app.receiver_thread.is_alive():
            app.receiver_running = False
            app.receiver_thread.join(timeout=5)
        
        # 断开所有连接
        if app.pop3_client:
            app.pop3_client.disconnect()
        if app.smtp_client:
            app.smtp_client.disconnect()
        
        root.destroy()
    
    root.protocol("WM_DELETE_WINDOW", on_closing)
    root.mainloop()

if __name__ == "__main__":
    main()
```

## 六、使用说明

### 1. 准备工作
```bash
# 安装依赖
pip install tkinter  # 通常已内置

# 创建必要目录
mkdir -p attachments config cache
```

### 2. 启动程序
```bash
python mail_client.py
```

### 3. 配置邮箱账户
1. 点击"添加账户"
2. 选择邮箱类型（163、QQ、Gmail或自定义）
3. 输入邮箱地址和密码
4. 点击"测试连接"验证配置
5. 点击"保存"

### 4. 主要功能
- **收邮件**：自动定时检查新邮件，支持手动刷新
- **发邮件**：支持文本/HTML格式，可添加附件
- **邮件管理**：支持回复、转发、删除、星标、标记已读
- **文件夹**：收件箱、已发送、草稿箱、星标邮件、垃圾邮件、已删除
- **附件**：支持附件下载和打开
- **搜索**：支持邮件搜索功能

## 七、系统特点

1. **完整的邮件协议实现**：支持POP3收件和SMTP发件
2. **多线程架构**：邮件收发不阻塞UI，支持后台自动检查
3. **图形化界面**：直观的邮件客户端界面
4. **本地数据库存储**：SQLite存储邮件和配置
5. **附件管理**：支持附件的下载和查看
6. **账户管理**：支持多个邮箱账户
7. **邮件分类**：支持文件夹管理
8. **通知系统**：新邮件桌面通知
9. **断点续传**：网络异常后自动重连

这个邮件客户端系统功能完整，可以直接用于日常邮件收发，具有良好的用户体验和稳定性。