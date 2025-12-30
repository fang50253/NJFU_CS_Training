import json
import socket
import threading
import sqlite3
import hashlib
import time
import os
from datetime import datetime
from typing import Dict, List, Set
import base64
import struct

class ChatServer:
    def __init__(self, host='0.0.0.0', port=8888, file_port=8889):
        self.host = host
        self.port = port
        self.file_port = file_port
        self.broadcast_port = 9999
        self.heartbeat_port = 7777
        
        # 数据存储
        self.db_file = 'chat_server.db'
        self.file_storage = './chat_files'
        
        # 在线用户管理
        self.online_users: Dict[str, socket.socket] = {}  # user_id -> socket
        self.user_info: Dict[str, dict] = {}  # user_id -> user_info
        self.user_locations: Dict[str, str] = {}  # user_id -> IP地址
        
        # 好友关系
        self.friend_requests: Dict[str, List[dict]] = {}  # user_id -> 待处理请求
        
        # 初始化
        self.init_database()
        self.init_file_storage()
        
        # 启动UDP广播服务器
        self.start_broadcast_server()
        self.start_heartbeat_server()
        
    def init_database(self):
        """初始化数据库"""
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        # 用户表
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS users (
                user_id TEXT PRIMARY KEY,
                username TEXT UNIQUE NOT NULL,
                password_hash TEXT NOT NULL,
                nickname TEXT,
                avatar TEXT,
                signature TEXT,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                last_login TIMESTAMP
            )
        ''')
        
        # 好友关系表
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS friends (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id1 TEXT NOT NULL,
                user_id2 TEXT NOT NULL,
                status INTEGER DEFAULT 1, -- 1: 好友, 0: 已删除
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                UNIQUE(user_id1, user_id2)
            )
        ''')
        
        # 好友请求表
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS friend_requests (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                from_user TEXT NOT NULL,
                to_user TEXT NOT NULL,
                message TEXT,
                status INTEGER DEFAULT 0, -- 0: 待处理, 1: 接受, 2: 拒绝
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        ''')
        
        # 消息表
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS messages (
                message_id TEXT PRIMARY KEY,
                from_user TEXT NOT NULL,
                to_user TEXT NOT NULL,
                message_type TEXT NOT NULL,
                content TEXT,
                file_path TEXT,
                is_read INTEGER DEFAULT 0,
                timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        ''')
        
        # 群组表
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS groups (
                group_id TEXT PRIMARY KEY,
                group_name TEXT NOT NULL,
                creator TEXT NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        ''')
        
        # 群组成员表
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS group_members (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                group_id TEXT NOT NULL,
                user_id TEXT NOT NULL,
                role INTEGER DEFAULT 0, -- 0: 成员, 1: 管理员, 2: 创建者
                joined_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                UNIQUE(group_id, user_id)
            )
        ''')
        
        conn.commit()
        conn.close()
    
    def init_file_storage(self):
        """初始化文件存储目录"""
        os.makedirs(self.file_storage, exist_ok=True)
        os.makedirs(os.path.join(self.file_storage, 'avatars'), exist_ok=True)
        os.makedirs(os.path.join(self.file_storage, 'images'), exist_ok=True)
        os.makedirs(os.path.join(self.file_storage, 'files'), exist_ok=True)
    
    def start_broadcast_server(self):
        """启动UDP广播服务器"""
        def broadcast_handler():
            broadcast_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            broadcast_sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            broadcast_sock.bind(('0.0.0.0', self.broadcast_port))
            
            print(f"广播服务器启动在端口 {self.broadcast_port}")
            
            while True:
                try:
                    data, addr = broadcast_sock.recvfrom(1024)
                    message = json.loads(data.decode('utf-8'))
                    
                    if message.get('type') == 'DISCOVER':
                        # 回应发现请求
                        response = {
                            'protocol': 'LCP/1.0',
                            'type': 'DISCOVER_RESPONSE',
                            'server_ip': self.host,
                            'server_port': self.port,
                            'timestamp': datetime.now().isoformat()
                        }
                        broadcast_sock.sendto(json.dumps(response).encode('utf-8'), addr)
                        
                except Exception as e:
                    print(f"广播服务器错误: {str(e)}")
        
        thread = threading.Thread(target=broadcast_handler, daemon=True)
        thread.start()
    
    def start_heartbeat_server(self):
        """启动心跳服务器"""
        def heartbeat_handler():
            heartbeat_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            heartbeat_sock.bind(('0.0.0.0', self.heartbeat_port))
            
            print(f"心跳服务器启动在端口 {self.heartbeat_port}")
            
            while True:
                try:
                    data, addr = heartbeat_sock.recvfrom(1024)
                    message = json.loads(data.decode('utf-8'))
                    
                    if message.get('type') == 'HEARTBEAT':
                        user_id = message.get('from')
                        if user_id:
                            # 更新用户位置和活跃时间
                            self.user_locations[user_id] = addr[0]
                            
                            # 广播在线状态给好友
                            self.broadcast_online_status(user_id, True)
                            
                except Exception as e:
                    print(f"心跳服务器错误: {str(e)}")
        
        thread = threading.Thread(target=heartbeat_handler, daemon=True)
        thread.start()
    
    def start(self):
        """启动主服务器"""
        # 启动TCP服务器
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((self.host, self.port))
        server.listen(20)
        
        print(f"聊天服务器启动在 {self.host}:{self.port}")
        
        # 启动文件传输服务器
        self.start_file_server()
        
        # 接受客户端连接
        while True:
            client, addr = server.accept()
            thread = threading.Thread(target=self.handle_client, args=(client, addr))
            thread.daemon = True
            thread.start()
    
    def start_file_server(self):
        """启动文件传输服务器"""
        def file_server_handler():
            file_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            file_server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            file_server.bind((self.host, self.file_port))
            file_server.listen(10)
            
            print(f"文件服务器启动在端口 {self.file_port}")
            
            while True:
                client, addr = file_server.accept()
                thread = threading.Thread(target=self.handle_file_transfer, args=(client, addr))
                thread.daemon = True
                thread.start()
        
        thread = threading.Thread(target=file_server_handler, daemon=True)
        thread.start()
    
    def handle_client(self, client, addr):
        """处理客户端连接"""
        user_id = None
        
        try:
            while True:
                # 接收消息
                try:
                    data = self.receive_message(client)
                    if not data:
                        break
                    
                    message = json.loads(data)
                    msg_type = message.get('type')
                    
                    # 根据消息类型处理
                    if msg_type == 'REGISTER':
                        response = self.handle_register(message, addr)
                    elif msg_type == 'LOGIN':
                        response, user_id = self.handle_login(message, client, addr)
                    elif msg_type == 'LOGOUT':
                        response = self.handle_logout(message)
                        if message.get('from') == user_id:
                            break
                    elif msg_type == 'SEARCH_USER':
                        response = self.handle_search_user(message)
                    elif msg_type == 'ADD_FRIEND':
                        response = self.handle_add_friend(message)
                    elif msg_type in ['ACCEPT_FRIEND', 'REJECT_FRIEND']:
                        response = self.handle_friend_response(message)
                    elif msg_type == 'DELETE_FRIEND':
                        response = self.handle_delete_friend(message)
                    elif msg_type == 'TEXT_MSG':
                        response = self.handle_text_message(message)
                    elif msg_type == 'IMAGE_MSG':
                        response = self.handle_image_message(message)
                    elif msg_type == 'GET_FRIENDS':
                        response = self.handle_get_friends(message)
                    elif msg_type == 'GET_MESSAGES':
                        response = self.handle_get_messages(message)
                    elif msg_type == 'HEARTBEAT':
                        response = self.handle_heartbeat(message, addr)
                    else:
                        response = self.create_response('ERROR', 
                                                       {'error': 'Unknown message type'})
                    
                    # 发送响应
                    if response:
                        self.send_message(client, response)
                        
                except json.JSONDecodeError:
                    error_msg = self.create_response('ERROR', {'error': 'Invalid JSON'})
                    self.send_message(client, error_msg)
                except Exception as e:
                    error_msg = self.create_response('ERROR', {'error': str(e)})
                    self.send_message(client, error_msg)
                    
        except Exception as e:
            print(f"客户端 {addr} 错误: {str(e)}")
        finally:
            # 清理资源
            if user_id and user_id in self.online_users:
                del self.online_users[user_id]
                del self.user_locations[user_id]
                # 广播离线状态
                self.broadcast_online_status(user_id, False)
            
            client.close()
            print(f"客户端 {addr} 断开连接")
    
    def handle_file_transfer(self, client, addr):
        """处理文件传输"""
        try:
            # 接收文件信息头
            header_data = b""
            while b'\r\n\r\n' not in header_data:
                chunk = client.recv(1)
                if not chunk:
                    break
                header_data += chunk
            
            if not header_data:
                return
            
            header_str = header_data.decode('utf-8')
            lines = header_str.split('\r\n')
            
            # 解析头部
            file_info = {}
            for line in lines:
                if ': ' in line:
                    key, value = line.split(': ', 1)
                    file_info[key] = value
            
            file_name = file_info.get('File-Name')
            file_size = int(file_info.get('File-Size', 0))
            file_type = file_info.get('File-Type')
            user_id = file_info.get('User-ID')
            
            if not file_name or file_size == 0:
                return
            
            # 确定保存路径
            if file_type == 'avatar':
                save_dir = 'avatars'
            elif file_type == 'image':
                save_dir = 'images'
            else:
                save_dir = 'files'
            
            save_path = os.path.join(self.file_storage, save_dir, f"{user_id}_{file_name}")
            
            # 接收文件数据
            received = 0
            with open(save_path, 'wb') as f:
                while received < file_size:
                    chunk = client.recv(min(4096, file_size - received))
                    if not chunk:
                        break
                    f.write(chunk)
                    received += len(chunk)
            
            # 发送确认
            response = {
                'protocol': 'LCP/1.0',
                'type': 'FILE_UPLOAD_SUCCESS',
                'file_path': save_path,
                'file_url': f"/files/{save_dir}/{user_id}_{file_name}",
                'timestamp': datetime.now().isoformat()
            }
            
            self.send_message(client, response)
            
        except Exception as e:
            print(f"文件传输错误: {str(e)}")
        finally:
            client.close()
    
    def handle_register(self, message, addr):
        """处理用户注册"""
        username = message.get('content', {}).get('username')
        password = message.get('content', {}).get('password')
        nickname = message.get('content', {}).get('nickname', username)
        
        if not username or not password:
            return self.create_response('ERROR', {'error': '用户名和密码不能为空'})
        
        # 检查用户名是否已存在
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        cursor.execute("SELECT user_id FROM users WHERE username = ?", (username,))
        if cursor.fetchone():
            conn.close()
            return self.create_response('ERROR', {'error': '用户名已存在'})
        
        # 创建用户
        user_id = hashlib.md5(f"{username}{datetime.now()}".encode()).hexdigest()[:8]
        password_hash = hashlib.sha256(password.encode()).hexdigest()
        
        cursor.execute('''
            INSERT INTO users (user_id, username, password_hash, nickname)
            VALUES (?, ?, ?, ?)
        ''', (user_id, username, password_hash, nickname))
        
        conn.commit()
        conn.close()
        
        return self.create_response('REGISTER_SUCCESS', {
            'user_id': user_id,
            'username': username,
            'nickname': nickname
        })
    
    def handle_login(self, message, client, addr):
        """处理用户登录"""
        username = message.get('content', {}).get('username')
        password = message.get('content', {}).get('password')
        
        if not username or not password:
            return self.create_response('ERROR', {'error': '用户名和密码不能为空'}), None
        
        # 验证用户
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT user_id, password_hash, nickname FROM users 
            WHERE username = ?
        ''', (username,))
        
        result = cursor.fetchone()
        if not result:
            conn.close()
            return self.create_response('ERROR', {'error': '用户不存在'}), None
        
        user_id, password_hash, nickname = result
        
        # 验证密码
        if hashlib.sha256(password.encode()).hexdigest() != password_hash:
            conn.close()
            return self.create_response('ERROR', {'error': '密码错误'}), None
        
        # 更新最后登录时间
        cursor.execute('''
            UPDATE users SET last_login = ? WHERE user_id = ?
        ''', (datetime.now().isoformat(), user_id))
        
        conn.commit()
        conn.close()
        
        # 保存在线状态
        self.online_users[user_id] = client
        self.user_info[user_id] = {
            'username': username,
            'nickname': nickname,
            'login_time': datetime.now().isoformat(),
            'ip_address': addr[0]
        }
        self.user_locations[user_id] = addr[0]
        
        # 获取好友列表
        friends = self.get_user_friends(user_id)
        
        # 获取未读消息
        unread_messages = self.get_unread_messages(user_id)
        
        return self.create_response('LOGIN_SUCCESS', {
            'user_id': user_id,
            'nickname': nickname,
            'friends': friends,
            'unread_messages': unread_messages
        }), user_id
    
    def handle_logout(self, message):
        """处理用户登出"""
        user_id = message.get('from')
        
        if user_id in self.online_users:
            del self.online_users[user_id]
            del self.user_info[user_id]
            
            if user_id in self.user_locations:
                del self.user_locations[user_id]
            
            # 广播离线状态
            self.broadcast_online_status(user_id, False)
        
        return self.create_response('LOGOUT_SUCCESS', {})
    
    def handle_search_user(self, message):
        """搜索用户"""
        keyword = message.get('content', {}).get('keyword')
        search_type = message.get('content', {}).get('type', 'username')  # username or user_id
        
        if not keyword:
            return self.create_response('ERROR', {'error': '搜索关键词不能为空'})
        
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        if search_type == 'username':
            cursor.execute('''
                SELECT user_id, username, nickname, signature 
                FROM users 
                WHERE username LIKE ? OR nickname LIKE ?
                LIMIT 20
            ''', (f'%{keyword}%', f'%{keyword}%'))
        else:
            cursor.execute('''
                SELECT user_id, username, nickname, signature 
                FROM users 
                WHERE user_id = ?
            ''', (keyword,))
        
        results = []
        for row in cursor.fetchall():
            user_id, username, nickname, signature = row
            
            # 检查是否为好友
            cursor.execute('''
                SELECT status FROM friends 
                WHERE (user_id1 = ? AND user_id2 = ?) 
                   OR (user_id1 = ? AND user_id2 = ?)
            ''', (message.get('from'), user_id, user_id, message.get('from')))
            
            friend_status = cursor.fetchone()
            is_friend = friend_status is not None and friend_status[0] == 1
            
            # 检查是否有待处理的好友请求
            cursor.execute('''
                SELECT status FROM friend_requests 
                WHERE from_user = ? AND to_user = ? 
                   OR from_user = ? AND to_user = ?
            ''', (message.get('from'), user_id, user_id, message.get('from')))
            
            request = cursor.fetchone()
            request_status = request[0] if request else -1
            
            results.append({
                'user_id': user_id,
                'username': username,
                'nickname': nickname,
                'signature': signature,
                'is_online': user_id in self.online_users,
                'is_friend': is_friend,
                'request_status': request_status  # -1: 无请求, 0: 待处理, 1: 已接受, 2: 已拒绝
            })
        
        conn.close()
        
        return self.create_response('SEARCH_RESULT', {
            'results': results,
            'count': len(results)
        })
    
    def handle_add_friend(self, message):
        """添加好友"""
        from_user = message.get('from')
        to_user = message.get('content', {}).get('user_id')
        request_msg = message.get('content', {}).get('message', '')
        
        if not from_user or not to_user:
            return self.create_response('ERROR', {'error': '用户ID不能为空'})
        
        if from_user == to_user:
            return self.create_response('ERROR', {'error': '不能添加自己为好友'})
        
        # 检查是否已经是好友
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT status FROM friends 
            WHERE (user_id1 = ? AND user_id2 = ?) 
               OR (user_id1 = ? AND user_id2 = ?)
        ''', (from_user, to_user, to_user, from_user))
        
        if cursor.fetchone():
            conn.close()
            return self.create_response('ERROR', {'error': '已经是好友'})
        
        # 检查是否有待处理的请求
        cursor.execute('''
            SELECT status FROM friend_requests 
            WHERE from_user = ? AND to_user = ? AND status = 0
        ''', (from_user, to_user))
        
        if cursor.fetchone():
            conn.close()
            return self.create_response('ERROR', {'error': '已发送过好友请求'})
        
        # 创建好友请求
        request_id = hashlib.md5(f"{from_user}{to_user}{datetime.now()}".encode()).hexdigest()[:12]
        
        cursor.execute('''
            INSERT INTO friend_requests (from_user, to_user, message)
            VALUES (?, ?, ?)
        ''', (from_user, to_user, request_msg))
        
        conn.commit()
        conn.close()
        
        # 如果对方在线，实时通知
        if to_user in self.online_users:
            notification = self.create_response('FRIEND_REQUEST', {
                'from_user': from_user,
                'from_nickname': self.get_user_nickname(from_user),
                'message': request_msg,
                'request_time': datetime.now().isoformat()
            }, from_user=from_user, to_user=to_user)
            
            self.send_message(self.online_users[to_user], notification)
        
        return self.create_response('FRIEND_REQUEST_SENT', {
            'to_user': to_user,
            'message': request_msg
        })
    
    def handle_friend_response(self, message):
        """处理好友请求响应"""
        response_type = message.get('type')  # ACCEPT_FRIEND or REJECT_FRIEND
        from_user = message.get('from')  # 响应者
        request_id = message.get('content', {}).get('request_id')
        
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        # 获取请求信息
        cursor.execute('''
            SELECT from_user, to_user FROM friend_requests 
            WHERE id = ? AND to_user = ? AND status = 0
        ''', (request_id, from_user))
        
        result = cursor.fetchone()
        if not result:
            conn.close()
            return self.create_response('ERROR', {'error': '请求不存在或已处理'})
        
        requester, target = result
        
        # 更新请求状态
        new_status = 1 if response_type == 'ACCEPT_FRIEND' else 2
        cursor.execute('''
            UPDATE friend_requests SET status = ? WHERE id = ?
        ''', (new_status, request_id))
        
        if response_type == 'ACCEPT_FRIEND':
            # 添加好友关系
            cursor.execute('''
                INSERT INTO friends (user_id1, user_id2, status)
                VALUES (?, ?, 1)
            ''', (requester, target))
            
            # 通知双方
            if requester in self.online_users:
                accept_notice = self.create_response('FRIEND_ADDED', {
                    'friend_id': target,
                    'friend_nickname': self.get_user_nickname(target),
                    'timestamp': datetime.now().isoformat()
                }, from_user=target, to_user=requester)
                
                self.send_message(self.online_users[requester], accept_notice)
            
            if target in self.online_users:
                accept_notice = self.create_response('FRIEND_ADDED', {
                    'friend_id': requester,
                    'friend_nickname': self.get_user_nickname(requester),
                    'timestamp': datetime.now().isoformat()
                }, from_user=requester, to_user=target)
                
                self.send_message(self.online_users[target], accept_notice)
        
        conn.commit()
        conn.close()
        
        return self.create_response(f'{response_type}_SUCCESS', {
            'requester': requester,
            'response': 'accepted' if response_type == 'ACCEPT_FRIEND' else 'rejected'
        })
    
    def handle_delete_friend(self, message):
        """删除好友"""
        user_id = message.get('from')
        friend_id = message.get('content', {}).get('friend_id')
        
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        # 删除好友关系
        cursor.execute('''
            DELETE FROM friends 
            WHERE (user_id1 = ? AND user_id2 = ?) 
               OR (user_id1 = ? AND user_id2 = ?)
        ''', (user_id, friend_id, friend_id, user_id))
        
        conn.commit()
        conn.close()
        
        # 通知对方（如果在线）
        if friend_id in self.online_users:
            notification = self.create_response('FRIEND_DELETED', {
                'friend_id': user_id,
                'friend_nickname': self.get_user_nickname(user_id),
                'timestamp': datetime.now().isoformat()
            }, from_user=user_id, to_user=friend_id)
            
            self.send_message(self.online_users[friend_id], notification)
        
        return self.create_response('FRIEND_DELETED_SUCCESS', {
            'friend_id': friend_id
        })
    
    def handle_text_message(self, message):
        """处理文本消息"""
        from_user = message.get('from')
        to_user = message.get('to')
        content = message.get('content', {}).get('text', '')
        
        if not content or not to_user:
            return self.create_response('ERROR', {'error': '消息内容或接收者不能为空'})
        
        # 生成消息ID
        message_id = hashlib.md5(f"{from_user}{to_user}{datetime.now()}".encode()).hexdigest()[:16]
        
        # 保存消息到数据库
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        cursor.execute('''
            INSERT INTO messages (message_id, from_user, to_user, message_type, content)
            VALUES (?, ?, ?, ?, ?)
        ''', (message_id, from_user, to_user, 'TEXT_MSG', content))
        
        conn.commit()
        conn.close()
        
        # 构建转发消息
        forward_message = self.create_response('TEXT_MSG', {
            'text': content,
            'message_id': message_id,
            'timestamp': datetime.now().isoformat()
        }, from_user=from_user, to_user=to_user)
        
        # 如果接收者在线，实时转发
        if to_user in self.online_users:
            try:
                self.send_message(self.online_users[to_user], forward_message)
                return self.create_response('MESSAGE_SENT', {
                    'message_id': message_id,
                    'to_user': to_user,
                    'status': 'delivered'
                })
            except:
                return self.create_response('MESSAGE_SENT', {
                    'message_id': message_id,
                    'to_user': to_user,
                    'status': 'saved'  # 保存但未送达
                })
        else:
            return self.create_response('MESSAGE_SENT', {
                'message_id': message_id,
                'to_user': to_user,
                'status': 'saved'  # 保存但对方离线
            })
    
    def handle_image_message(self, message):
        """处理图片消息"""
        from_user = message.get('from')
        to_user = message.get('to')
        image_data = message.get('content', {}).get('image_data')
        image_name = message.get('content', {}).get('image_name', 'image.jpg')
        
        if not image_data or not to_user:
            return self.create_response('ERROR', {'error': '图片数据或接收者不能为空'})
        
        # 解码图片
        try:
            image_bytes = base64.b64decode(image_data)
        except:
            return self.create_response('ERROR', {'error': '图片数据格式错误'})
        
        # 保存图片文件
        file_id = hashlib.md5(f"{from_user}{to_user}{datetime.now()}".encode()).hexdigest()[:12]
        file_ext = os.path.splitext(image_name)[1] or '.jpg'
        file_name = f"{file_id}{file_ext}"
        file_path = os.path.join(self.file_storage, 'images', file_name)
        
        with open(file_path, 'wb') as f:
            f.write(image_bytes)
        
        # 生成消息ID
        message_id = hashlib.md5(f"{from_user}{to_user}{file_id}".encode()).hexdigest()[:16]
        
        # 保存消息到数据库
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        cursor.execute('''
            INSERT INTO messages (message_id, from_user, to_user, message_type, content, file_path)
            VALUES (?, ?, ?, ?, ?, ?)
        ''', (message_id, from_user, to_user, 'IMAGE_MSG', image_name, file_path))
        
        conn.commit()
        conn.close()
        
        # 构建转发消息
        forward_message = self.create_response('IMAGE_MSG', {
            'image_name': image_name,
            'image_url': f"/images/{file_name}",
            'file_id': file_id,
            'message_id': message_id,
            'timestamp': datetime.now().isoformat()
        }, from_user=from_user, to_user=to_user)
        
        # 如果接收者在线，实时转发
        if to_user in self.online_users:
            try:
                self.send_message(self.online_users[to_user], forward_message)
                return self.create_response('MESSAGE_SENT', {
                    'message_id': message_id,
                    'to_user': to_user,
                    'status': 'delivered',
                    'file_id': file_id
                })
            except:
                return self.create_response('MESSAGE_SENT', {
                    'message_id': message_id,
                    'to_user': to_user,
                    'status': 'saved',
                    'file_id': file_id
                })
        else:
            return self.create_response('MESSAGE_SENT', {
                'message_id': message_id,
                'to_user': to_user,
                'status': 'saved',
                'file_id': file_id
            })
    
    def handle_get_friends(self, message):
        """获取好友列表"""
        user_id = message.get('from')
        
        friends = self.get_user_friends(user_id)
        
        return self.create_response('FRIENDS_LIST', {
            'friends': friends,
            'count': len(friends)
        })
    
    def handle_get_messages(self, message):
        """获取消息记录"""
        user_id = message.get('from')
        target_id = message.get('content', {}).get('target_id')
        limit = message.get('content', {}).get('limit', 50)
        offset = message.get('content', {}).get('offset', 0)
        
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT message_id, from_user, to_user, message_type, content, 
                   file_path, is_read, timestamp
            FROM messages
            WHERE (from_user = ? AND to_user = ?) 
               OR (from_user = ? AND to_user = ?)
            ORDER BY timestamp DESC
            LIMIT ? OFFSET ?
        ''', (user_id, target_id, target_id, user_id, limit, offset))
        
        messages = []
        for row in cursor.fetchall():
            msg_id, from_user, to_user, msg_type, content, file_path, is_read, timestamp = row
            
            message_data = {
                'message_id': msg_id,
                'from_user': from_user,
                'to_user': to_user,
                'type': msg_type,
                'content': content,
                'is_read': bool(is_read),
                'timestamp': timestamp
            }
            
            if file_path:
                # 提取相对路径
                rel_path = os.path.relpath(file_path, self.file_storage)
                message_data['file_url'] = f"/files/{rel_path.replace(os.sep, '/')}"
            
            messages.append(message_data)
        
        conn.close()
        
        # 标记为已读
        self.mark_messages_as_read(user_id, target_id)
        
        return self.create_response('MESSAGES_HISTORY', {
            'target_id': target_id,
            'messages': messages,
            'count': len(messages)
        })
    
    def handle_heartbeat(self, message, addr):
        """处理心跳"""
        user_id = message.get('from')
        
        if user_id:
            self.user_locations[user_id] = addr[0]
            self.broadcast_online_status(user_id, True)
        
        return self.create_response('HEARTBEAT_ACK', {
            'timestamp': datetime.now().isoformat()
        })
    
    def get_user_friends(self, user_id):
        """获取用户的好友列表"""
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT u.user_id, u.username, u.nickname, u.avatar, u.signature
            FROM users u
            INNER JOIN friends f ON (f.user_id2 = u.user_id OR f.user_id1 = u.user_id)
            WHERE (f.user_id1 = ? OR f.user_id2 = ?) 
              AND f.status = 1 
              AND u.user_id != ?
        ''', (user_id, user_id, user_id))
        
        friends = []
        for row in cursor.fetchall():
            friend_id, username, nickname, avatar, signature = row
            
            friends.append({
                'user_id': friend_id,
                'username': username,
                'nickname': nickname or username,
                'avatar': avatar,
                'signature': signature or '',
                'is_online': friend_id in self.online_users,
                'last_seen': self.user_locations.get(friend_id, '')
            })
        
        conn.close()
        
        # 按昵称排序
        friends.sort(key=lambda x: x['nickname'].upper())
        
        return friends
    
    def get_unread_messages(self, user_id):
        """获取未读消息"""
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        cursor.execute('''
            SELECT from_user, COUNT(*) as unread_count
            FROM messages
            WHERE to_user = ? AND is_read = 0
            GROUP BY from_user
        ''', (user_id,))
        
        unread = {}
        for row in cursor.fetchall():
            from_user, count = row
            unread[from_user] = count
        
        conn.close()
        
        return unread
    
    def mark_messages_as_read(self, user_id, target_id):
        """标记消息为已读"""
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        cursor.execute('''
            UPDATE messages 
            SET is_read = 1 
            WHERE from_user = ? AND to_user = ? AND is_read = 0
        ''', (target_id, user_id))
        
        conn.commit()
        conn.close()
    
    def get_user_nickname(self, user_id):
        """获取用户昵称"""
        conn = sqlite3.connect(self.db_file)
        cursor = conn.cursor()
        
        cursor.execute('SELECT nickname FROM users WHERE user_id = ?', (user_id,))
        result = cursor.fetchone()
        
        conn.close()
        
        return result[0] if result else user_id
    
    def broadcast_online_status(self, user_id, is_online):
        """广播用户在线状态给好友"""
        friends = self.get_user_friends(user_id)
        
        status_message = self.create_response('ONLINE_STATUS', {
            'user_id': user_id,
            'is_online': is_online,
            'timestamp': datetime.now().isoformat()
        })
        
        for friend in friends:
            if friend['user_id'] in self.online_users:
                try:
                    self.send_message(self.online_users[friend['user_id']], status_message)
                except:
                    pass
    
    def create_response(self, msg_type, content, from_user=None, to_user=None):
        """创建响应消息"""
        response = {
            'protocol': 'LCP/1.0',
            'type': msg_type,
            'timestamp': datetime.now().isoformat(),
            'content': content
        }
        
        if from_user:
            response['from'] = from_user
        if to_user:
            response['to'] = to_user
        
        return json.dumps(response, ensure_ascii=False)
    
    def send_message(self, sock, message):
        """发送消息"""
        if isinstance(message, dict):
            message = json.dumps(message, ensure_ascii=False)
        
        message_bytes = message.encode('utf-8')
        
        # 添加长度前缀
        length = len(message_bytes)
        sock.send(struct.pack('!I', length))
        sock.send(message_bytes)
    
    def receive_message(self, sock):
        """接收消息"""
        try:
            # 读取消息长度
            length_data = sock.recv(4)
            if len(length_data) < 4:
                return None
            
            length = struct.unpack('!I', length_data)[0]
            
            # 读取消息内容
            chunks = []
            bytes_received = 0
            
            while bytes_received < length:
                chunk = sock.recv(min(length - bytes_received, 4096))
                if not chunk:
                    break
                chunks.append(chunk)
                bytes_received += len(chunk)
            
            if bytes_received < length:
                return None
            
            return b''.join(chunks).decode('utf-8')
            
        except:
            return None

if __name__ == "__main__":
    server = ChatServer(port=8888)
    server.start()