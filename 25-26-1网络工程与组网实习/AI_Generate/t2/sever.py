import socket
import threading
import os
import json
import hashlib
from datetime import datetime
import struct

class SFTPServer:
    def __init__(self, host='0.0.0.0', port=2121):
        self.host = host
        self.port = port
        self.root_dir = './server_files'
        self.users = {
            'admin': 'admin123',
            'user': 'user123',
            'guest': 'guest'
        }
        self.sessions = {}  # 客户端地址 -> 会话信息
        self.transfer_type = 'I'  # 默认二进制传输
        self.passive_ports = {}  # 被动模式端口映射
        
        # 确保根目录存在
        os.makedirs(self.root_dir, exist_ok=True)
        
    def start(self):
        """启动服务器"""
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((self.host, self.port))
        server.listen(5)
        print(f"SFTP服务器启动在 {self.host}:{self.port}")
        print(f"根目录: {os.path.abspath(self.root_dir)}")
        
        while True:
            client, address = server.accept()
            session_id = f"{address[0]}:{address[1]}"
            self.sessions[session_id] = {
                'authenticated': False,
                'username': None,
                'current_dir': '/',
                'data_port': None,
                'data_socket': None,
                'last_active': datetime.now()
            }
            
            # 发送欢迎消息
            welcome_msg = "220 Welcome to Simple FTP Server\r\n"
            client.send(welcome_msg.encode('utf-8'))
            
            thread = threading.Thread(target=self.handle_client, 
                                    args=(client, address, session_id))
            thread.start()
    
    def handle_client(self, client, address, session_id):
        """处理客户端连接"""
        try:
            while True:
                # 接收命令
                try:
                    command = client.recv(1024).decode('utf-8').strip()
                except:
                    break
                    
                if not command:
                    break
                
                # 更新活动时间
                self.sessions[session_id]['last_active'] = datetime.now()
                
                # 解析命令
                parts = command.split(' ', 1)
                cmd = parts[0].upper() if parts else ''
                arg = parts[1] if len(parts) > 1 else ''
                
                # 处理命令
                response = self.process_command(cmd, arg, session_id, client, address)
                client.send(response.encode('utf-8'))
                
                # 如果是QUIT命令，结束连接
                if cmd == 'QUIT':
                    break
                    
        except Exception as e:
            print(f"客户端 {address} 错误: {str(e)}")
        finally:
            # 清理会话
            if session_id in self.sessions:
                # 关闭数据连接
                data_socket = self.sessions[session_id].get('data_socket')
                if data_socket:
                    try:
                        data_socket.close()
                    except:
                        pass
                del self.sessions[session_id]
            
            client.close()
            print(f"客户端 {address} 断开连接")
    
    def process_command(self, cmd, arg, session_id, client, address):
        """处理具体命令"""
        session = self.sessions[session_id]
        
        # 需要认证的命令（除了USER, PASS, QUIT, FEAT, SYST）
        if cmd not in ['USER', 'PASS', 'QUIT', 'FEAT', 'SYST'] and not session['authenticated']:
            return "530 Not logged in\r\n"
        
        if cmd == 'USER':
            return self.handle_user(arg, session_id)
        elif cmd == 'PASS':
            return self.handle_pass(arg, session_id)
        elif cmd == 'QUIT':
            return self.handle_quit(session_id)
        elif cmd == 'LIST':
            return self.handle_list(arg, session_id, address)
        elif cmd == 'CWD':
            return self.handle_cwd(arg, session_id)
        elif cmd == 'PWD':
            return self.handle_pwd(session_id)
        elif cmd == 'MKD':
            return self.handle_mkd(arg, session_id)
        elif cmd == 'RMD':
            return self.handle_rmd(arg, session_id)
        elif cmd == 'STOR':
            return self.handle_stor(arg, session_id, address)
        elif cmd == 'RETR':
            return self.handle_retr(arg, session_id, address)
        elif cmd == 'DELE':
            return self.handle_dele(arg, session_id)
        elif cmd == 'SIZE':
            return self.handle_size(arg, session_id)
        elif cmd == 'TYPE':
            return self.handle_type(arg)
        elif cmd == 'PASV':
            return self.handle_pasv(session_id, address)
        elif cmd == 'FEAT':
            return self.handle_feat()
        elif cmd == 'SYST':
            return self.handle_syst()
        else:
            return "500 Unknown command\r\n"
    
    def handle_user(self, username, session_id):
        """处理USER命令"""
        if username in self.users:
            self.sessions[session_id]['username'] = username
            return "331 User name okay, need password\r\n"
        else:
            return "530 User not found\r\n"
    
    def handle_pass(self, password, session_id):
        """处理PASS命令"""
        username = self.sessions[session_id].get('username')
        if username and self.users.get(username) == password:
            self.sessions[session_id]['authenticated'] = True
            return "230 User logged in\r\n"
        else:
            return "530 Login incorrect\r\n"
    
    def handle_quit(self, session_id):
        """处理QUIT命令"""
        return "221 Goodbye\r\n"
    
    def handle_list(self, path, session_id, address):
        """处理LIST命令"""
        try:
            # 建立数据连接
            data_socket = self.get_data_connection(session_id, address)
            if not data_socket:
                return "425 Can't open data connection\r\n"
            
            # 获取目录路径
            dir_path = self.get_absolute_path(path, session_id)
            
            if not os.path.exists(dir_path):
                return "550 Directory not found\r\n"
            
            # 发送列表
            files = []
            for item in os.listdir(dir_path):
                item_path = os.path.join(dir_path, item)
                stat = os.stat(item_path)
                
                # 格式化文件信息（类似ls -l的简化版）
                if os.path.isdir(item_path):
                    file_type = 'd'
                else:
                    file_type = '-'
                
                file_info = f"{file_type}rwxr-xr-x 1 owner group {stat.st_size} " \
                          f"{datetime.fromtimestamp(stat.st_mtime).strftime('%b %d %H:%M')} " \
                          f"{item}\r\n"
                files.append(file_info.encode('utf-8'))
            
            # 发送成功响应
            response = "150 Here comes the directory listing\r\n"
            
            # 发送文件列表
            for file_info in files:
                data_socket.send(file_info)
            
            data_socket.close()
            self.sessions[session_id]['data_socket'] = None
            
            return "226 Directory send OK\r\n"
            
        except Exception as e:
            return f"550 Error: {str(e)}\r\n"
    
    def handle_cwd(self, path, session_id):
        """处理CWD命令"""
        try:
            abs_path = self.get_absolute_path(path, session_id)
            if os.path.exists(abs_path) and os.path.isdir(abs_path):
                # 计算相对路径
                rel_path = os.path.relpath(abs_path, self.root_dir)
                if rel_path == '.':
                    rel_path = '/'
                else:
                    rel_path = '/' + rel_path
                
                self.sessions[session_id]['current_dir'] = rel_path
                return f"250 Directory changed to {rel_path}\r\n"
            else:
                return "550 Directory not found\r\n"
        except:
            return "550 Failed to change directory\r\n"
    
    def handle_pwd(self, session_id):
        """处理PWD命令"""
        current_dir = self.sessions[session_id]['current_dir']
        return f'257 "{current_dir}" is current directory\r\n'
    
    def handle_mkd(self, path, session_id):
        """处理MKD命令"""
        try:
            abs_path = self.get_absolute_path(path, session_id)
            os.makedirs(abs_path, exist_ok=False)
            
            # 计算相对路径
            rel_path = os.path.relpath(abs_path, self.root_dir)
            if rel_path == '.':
                rel_path = '/'
            else:
                rel_path = '/' + rel_path
            
            return f"257 \"{rel_path}\" created\r\n"
        except FileExistsError:
            return "550 Directory already exists\r\n"
        except:
            return "550 Failed to create directory\r\n"
    
    def handle_rmd(self, path, session_id):
        """处理RMD命令"""
        try:
            abs_path = self.get_absolute_path(path, session_id)
            if not os.path.exists(abs_path):
                return "550 Directory not found\r\n"
            
            if not os.path.isdir(abs_path):
                return "550 Not a directory\r\n"
            
            # 检查目录是否为空
            if os.listdir(abs_path):
                return "550 Directory not empty\r\n"
            
            os.rmdir(abs_path)
            return "250 Directory removed\r\n"
        except:
            return "550 Failed to remove directory\r\n"
    
    def handle_stor(self, filename, session_id, address):
        """处理STOR命令（上传文件）"""
        try:
            # 建立数据连接
            data_socket = self.get_data_connection(session_id, address)
            if not data_socket:
                return "425 Can't open data connection\r\n"
            
            # 获取绝对路径
            abs_path = self.get_absolute_path(filename, session_id)
            
            # 检查是否允许覆盖等（这里简单处理）
            response = "150 OK to send data\r\n"
            
            # 在后台线程中接收文件
            threading.Thread(target=self.receive_file, 
                           args=(data_socket, abs_path, session_id)).start()
            
            return response
            
        except Exception as e:
            return f"550 Error: {str(e)}\r\n"
    
    def handle_retr(self, filename, session_id, address):
        """处理RETR命令（下载文件）"""
        try:
            # 建立数据连接
            data_socket = self.get_data_connection(session_id, address)
            if not data_socket:
                return "425 Can't open data connection\r\n"
            
            # 获取绝对路径
            abs_path = self.get_absolute_path(filename, session_id)
            
            if not os.path.exists(abs_path):
                return "550 File not found\r\n"
            
            if not os.path.isfile(abs_path):
                return "550 Not a file\r\n"
            
            response = f"150 Opening {self.transfer_type} mode data connection for {filename}\r\n"
            
            # 在后台线程中发送文件
            threading.Thread(target=self.send_file, 
                           args=(data_socket, abs_path, session_id)).start()
            
            return response
            
        except Exception as e:
            return f"550 Error: {str(e)}\r\n"
    
    def handle_dele(self, filename, session_id):
        """处理DELE命令"""
        try:
            abs_path = self.get_absolute_path(filename, session_id)
            if not os.path.exists(abs_path):
                return "550 File not found\r\n"
            
            if not os.path.isfile(abs_path):
                return "550 Not a file\r\n"
            
            os.remove(abs_path)
            return "250 File deleted\r\n"
        except:
            return "550 Failed to delete file\r\n"
    
    def handle_size(self, filename, session_id):
        """处理SIZE命令"""
        try:
            abs_path = self.get_absolute_path(filename, session_id)
            if not os.path.exists(abs_path):
                return "550 File not found\r\n"
            
            size = os.path.getsize(abs_path)
            return f"213 {size}\r\n"
        except:
            return "550 Could not get file size\r\n"
    
    def handle_type(self, type_code):
        """处理TYPE命令"""
        if type_code.upper() in ['A', 'I']:
            self.transfer_type = type_code.upper()
            return f"200 Type set to {type_code}\r\n"
        else:
            return "500 Unrecognized type\r\n"
    
    def handle_pasv(self, session_id, address):
        """处理PASV命令（被动模式）"""
        try:
            # 创建数据socket
            data_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            data_socket.bind(('0.0.0.0', 0))  # 随机端口
            data_socket.listen(1)
            
            # 获取端口
            _, port = data_socket.getsockname()
            self.sessions[session_id]['data_socket'] = data_socket
            
            # 返回PASV响应（格式：h1,h2,h3,h4,p1,p2）
            ip_parts = address[0].split('.')
            p1 = port // 256
            p2 = port % 256
            
            response = f"227 Entering Passive Mode ({','.join(ip_parts)},{p1},{p2})\r\n"
            return response
            
        except:
            return "425 Can't open data connection\r\n"
    
    def handle_feat(self):
        """处理FEAT命令"""
        features = [
            "UTF8",
            "SIZE",
            "MDTM",
            "REST STREAM",
            "MLST type*;size*;modify*;",
            "MLSD"
        ]
        response = "211-Features:\r\n"
        response += "\r\n".join([f" {feat}" for feat in features])
        response += "\r\n211 End\r\n"
        return response
    
    def handle_syst(self):
        """处理SYST命令"""
        return "215 UNIX Type: L8\r\n"
    
    def get_absolute_path(self, path, session_id):
        """获取绝对路径"""
        if not path:
            path = self.sessions[session_id]['current_dir']
        
        # 处理相对路径
        if path.startswith('/'):
            # 绝对路径
            rel_path = path[1:]  # 去掉开头的'/'
        else:
            # 相对路径
            current_dir = self.sessions[session_id]['current_dir']
            if current_dir == '/':
                rel_path = path
            else:
                rel_path = os.path.join(current_dir[1:], path)
        
        # 清理路径中的..等
        rel_path = os.path.normpath(rel_path)
        
        # 确保路径在根目录内
        abs_path = os.path.abspath(os.path.join(self.root_dir, rel_path))
        root_abs = os.path.abspath(self.root_dir)
        
        if not abs_path.startswith(root_abs):
            raise ValueError("Access denied: outside root directory")
        
        return abs_path
    
    def get_data_connection(self, session_id, address):
        """获取数据连接"""
        data_socket = self.sessions[session_id].get('data_socket')
        if data_socket:
            try:
                client_socket, _ = data_socket.accept()
                return client_socket
            except:
                return None
        return None
    
    def receive_file(self, data_socket, file_path, session_id):
        """接收文件数据"""
        try:
            with open(file_path, 'wb') as f:
                while True:
                    data = data_socket.recv(4096)
                    if not data:
                        break
                    f.write(data)
            data_socket.close()
        except Exception as e:
            print(f"接收文件错误: {str(e)}")
    
    def send_file(self, data_socket, file_path, session_id):
        """发送文件数据"""
        try:
            with open(file_path, 'rb') as f:
                while True:
                    data = f.read(4096)
                    if not data:
                        break
                    data_socket.send(data)
            data_socket.close()
        except Exception as e:
            print(f"发送文件错误: {str(e)}")

if __name__ == "__main__":
    server = SFTPServer(port=2121)
    server.start()