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