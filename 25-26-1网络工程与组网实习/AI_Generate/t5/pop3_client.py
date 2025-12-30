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