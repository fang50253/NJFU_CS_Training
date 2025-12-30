import tkinter as tk
from tkinter import ttk, messagebox, filedialog, scrolledtext, PhotoImage
import socket
import json
import threading
import time
import os
import base64
import struct
import uuid
from datetime import datetime
from PIL import Image, ImageTk
import io

class ChatClientGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("局域网聊天系统")
        self.root.geometry("1000x700")
        
        # 连接状态
        self.connected = False
        self.sock = None
        self.file_sock = None
        self.server_ip = None
        self.server_port = 8888
        self.file_port = 8889
        self.broadcast_port = 9999
        self.heartbeat_port = 7777
        
        # 用户信息
        self.user_id = None
        self.username = None
        self.nickname = None
        
        # 数据存储
        self.friends = {}  # user_id -> friend_info
        self.messages = {}  # user_id -> message_list
        self.unread_counts = {}  # user_id -> unread_count
        
        # 局域网发现
        self.lan_users = []  # 局域网内发现的用户
        self.discovery_sock = None
        self.heartbeat_sock = None
        
        # 当前聊天对象
        self.current_chat = None
        
        # 初始化界面
        self.show_login_screen()
        
        # 启动局域网发现
        self.start_lan_discovery()
        
    def show_login_screen(self):
        """显示登录注册界面"""
        self.clear_window()
        
        # 标题
        title_label = tk.Label(self.root, text="局域网聊天系统", 
                              font=("微软雅黑", 24, "bold"))
        title_label.pack(pady=40)
        
        # 登录框
        login_frame = tk.Frame(self.root)
        login_frame.pack(pady=20)
        
        # 服务器地址
        server_frame = tk.Frame(login_frame)
        server_frame.pack(pady=5)
        
        tk.Label(server_frame, text="服务器:", font=("微软雅黑", 11)).pack(side='left', padx=5)
        self.server_var = tk.StringVar(value="localhost")
        tk.Entry(server_frame, textvariable=self.server_var, width=20, 
                font=("微软雅黑", 11)).pack(side='left', padx=5)
        
        # 用户名
        user_frame = tk.Frame(login_frame)
        user_frame.pack(pady=5)
        
        tk.Label(user_frame, text="用户名:", font=("微软雅黑", 11)).pack(side='left', padx=5)
        self.user_var = tk.StringVar()
        tk.Entry(user_frame, textvariable=self.user_var, width=20, 
                font=("微软雅黑", 11)).pack(side='left', padx=5)
        
        # 密码
        pass_frame = tk.Frame(login_frame)
        pass_frame.pack(pady=5)
        
        tk.Label(pass_frame, text="密码:", font=("微软雅黑", 11)).pack(side='left', padx=5)
        self.pass_var = tk.StringVar()
        tk.Entry(pass_frame, textvariable=self.pass_var, show="*", 
                width=20, font=("微软雅黑", 11)).pack(side='left', padx=5)
        
        # 按钮
        button_frame = tk.Frame(login_frame)
        button_frame.pack(pady=20)
        
        tk.Button(button_frame, text="登录", command=self.login, 
                 font=("微软雅黑", 11), width=10, bg="#4CAF50", fg="white").pack(side='left', padx=10)
        tk.Button(button_frame, text="注册", command=self.register, 
                 font=("微软雅黑", 11), width=10, bg="#2196F3", fg="white").pack(side='left', padx=10)
        
        # 局域网发现区域
        lan_frame = tk.LabelFrame(self.root, text="局域网用户", font=("微软雅黑", 11))
        lan_frame.pack(pady=20, padx=40, fill='x')
        
        self.lan_listbox = tk.Listbox(lan_frame, height=4, font=("微软雅黑", 10))
        self.lan_listbox.pack(padx=10, pady=10, fill='x')
        
        lan_button_frame = tk.Frame(lan_frame)
        lan_button_frame.pack(pady=5)
        
        tk.Button(lan_button_frame, text="刷新", command=self.refresh_lan_users,
                 font=("微软雅黑", 10)).pack(side='left', padx=5)
        tk.Button(lan_button_frame, text="连接", command=self.connect_lan_user,
                 font=("微软雅黑", 10)).pack(side='left', padx=5)
    
    def show_main_screen(self):
        """显示主界面"""
        self.clear_window()
        
        # 创建菜单栏
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)
        
        # 文件菜单
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="文件", menu=file_menu)
        file_menu.add_command(label="退出", command=self.logout)
        
        # 好友菜单
        friend_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="好友", menu=friend_menu)
        friend_menu.add_command(label="添加好友", command=self.show_add_friend)
        friend_menu.add_command(label="刷新列表", command=self.refresh_friends)
        
        # 创建主框架
        main_frame = tk.Frame(self.root)
        main_frame.pack(fill='both', expand=True)
        
        # 左侧联系人面板
        left_panel = tk.Frame(main_frame, width=250, bg="#F0F0F0")
        left_panel.pack(side='left', fill='y')
        left_panel.pack_propagate(False)
        
        # 用户信息
        user_info_frame = tk.Frame(left_panel, bg="#4CAF50", height=80)
        user_info_frame.pack(fill='x')
        user_info_frame.pack_propagate(False)
        
        tk.Label(user_info_frame, text=self.nickname or self.username, 
                font=("微软雅黑", 14, "bold"), bg="#4CAF50", fg="white").pack(pady=10)
        tk.Label(user_info_frame, text=f"ID: {self.user_id}", 
                font=("微软雅黑", 10), bg="#4CAF50", fg="white").pack()
        
        # 联系人列表选项卡
        contact_notebook = ttk.Notebook(left_panel)
        contact_notebook.pack(fill='both', expand=True, padx=5, pady=5)
        
        # 好友列表
        friend_frame = tk.Frame(contact_notebook)
        self.setup_friend_list(friend_frame)
        contact_notebook.add(friend_frame, text="好友")
        
        # 群聊列表
        group_frame = tk.Frame(contact_notebook)
        tk.Label(group_frame, text="群聊功能开发中...", 
                font=("微软雅黑", 12)).pack(pady=50)
        contact_notebook.add(group_frame, text="群聊")
        
        # 局域网用户
        lan_frame = tk.Frame(contact_notebook)
        self.setup_lan_list(lan_frame)
        contact_notebook.add(lan_frame, text="局域网")
        
        # 右侧聊天面板
        right_panel = tk.Frame(main_frame)
        right_panel.pack(side='right', fill='both', expand=True)
        
        # 聊天标题
        self.chat_title = tk.Label(right_panel, text="请选择聊天对象", 
                                  font=("微软雅黑", 14), bg="#E0E0E0", height=2)
        self.chat_title.pack(fill='x')
        
        # 消息显示区域
        chat_frame = tk.Frame(right_panel)
        chat_frame.pack(fill='both', expand=True)
        
        # 消息列表
        self.message_list = tk.Listbox(chat_frame, font=("微软雅黑", 11), 
                                      bg="white", selectmode='single')
        scrollbar = tk.Scrollbar(chat_frame)
        self.message_list.config(yscrollcommand=scrollbar.set)
        scrollbar.config(command=self.message_list.yview)
        
        self.message_list.pack(side='left', fill='both', expand=True)
        scrollbar.pack(side='right', fill='y')
        
        # 消息输入区域
        input_frame = tk.Frame(right_panel, height=100)
        input_frame.pack(fill='x')
        input_frame.pack_propagate(False)
        
        # 工具栏
        toolbar = tk.Frame(input_frame)
        toolbar.pack(fill='x', padx=5, pady=2)
        
        tk.Button(toolbar, text="图片", command=self.send_image, 
                 font=("微软雅黑", 9)).pack(side='left', padx=2)
        tk.Button(toolbar, text="文件", command=self.send_file, 
                 font=("微软雅黑", 9)).pack(side='left', padx=2)
        
        # 输入框
        self.input_text = tk.Text(input_frame, font=("微软雅黑", 11), height=4)
        input_scrollbar = tk.Scrollbar(input_frame, command=self.input_text.yview)
        self.input_text.config(yscrollcommand=input_scrollbar.set)
        
        self.input_text.pack(side='left', fill='both', expand=True, padx=(5, 0))
        input_scrollbar.pack(side='right', fill='y')
        
        # 发送按钮
        send_button = tk.Button(input_frame, text="发送", command=self.send_message,
                               font=("微软雅黑", 11), bg="#4CAF50", fg="white", width=8)
        send_button.pack(side='right', padx=5, pady=5)
        
        # 绑定事件
        self.input_text.bind('<Return>', self.on_enter_pressed)
        self.input_text.bind('<Shift-Return>', lambda e: 'break')
        
        # 启动消息接收线程
        self.start_receive_thread()
        self.start_heartbeat()
    
    def setup_friend_list(self, parent):
        """设置好友列表"""
        # 搜索框
        search_frame = tk.Frame(parent)
        search_frame.pack(fill='x', padx=5, pady=5)
        
        self.search_var = tk.StringVar()
        search_entry = tk.Entry(search_frame, textvariable=self.search_var, 
                               font=("微软雅黑", 10))
        search_entry.pack(side='left', fill='x', expand=True, padx=(0, 5))
        search_entry.bind('<KeyRelease>', self.filter_friends)
        
        tk.Button(search_frame, text="搜索", command=self.search_users,
                 font=("微软雅黑", 9)).pack(side='right')
        
        # 好友列表
        friend_list_frame = tk.Frame(parent)
        friend_list_frame.pack(fill='both', expand=True)
        
        # 使用Treeview显示好友
        columns = ('status', 'name', 'unread')
        self.friend_tree = ttk.Treeview(friend_list_frame, columns=columns, 
                                       show='tree', height=20)
        
        # 设置列宽
        self.friend_tree.column('#0', width=0, stretch=False)
        self.friend_tree.column('status', width=30, anchor='center')
        self.friend_tree.column('name', width=180)
        self.friend_tree.column('unread', width=40, anchor='center')
        
        # 滚动条
        scrollbar = ttk.Scrollbar(friend_list_frame, orient='vertical',
                                 command=self.friend_tree.yview)
        self.friend_tree.configure(yscrollcommand=scrollbar.set)
        
        self.friend_tree.pack(side='left', fill='both', expand=True)
        scrollbar.pack(side='right', fill='y')
        
        # 绑定双击事件
        self.friend_tree.bind('<Double-Button-1>', self.on_friend_double_click)
    
    def setup_lan_list(self, parent):
        """设置局域网用户列表"""
        tk.Label(parent, text="局域网发现的用户", 
                font=("微软雅黑", 11)).pack(pady=10)
        
        # 用户列表
        self.lan_user_list = tk.Listbox(parent, font=("微软雅黑", 10), height=15)
        scrollbar = tk.Scrollbar(parent, command=self.lan_user_list.yview)
        self.lan_user_list.config(yscrollcommand=scrollbar.set)
        
        self.lan_user_list.pack(side='left', fill='both', expand=True, padx=(10, 0))
        scrollbar.pack(side='right', fill='y')
        
        # 按钮
        button_frame = tk.Frame(parent)
        button_frame.pack(fill='x', padx=10, pady=10)
        
        tk.Button(button_frame, text="刷新", command=self.refresh_lan_users,
                 font=("微软雅黑", 10)).pack(side='left', padx=5)
        tk.Button(button_frame, text="添加好友", command=self.add_lan_user,
                 font=("微软雅黑", 10)).pack(side='left', padx=5)
    
    def login(self):
        """登录"""
        server_ip = self.server_var.get()
        username = self.user_var.get()
        password = self.pass_var.get()
        
        if not all([server_ip, username, password]):
            messagebox.showwarning("输入错误", "请填写完整信息")
            return
        
        try:
            # 连接服务器
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((server_ip, self.server_port))
            
            # 发送登录请求
            login_msg = {
                'protocol': 'LCP/1.0',
                'type': 'LOGIN',
                'timestamp': datetime.now().isoformat(),
                'content': {
                    'username': username,
                    'password': password
                }
            }
            
            self.send_message(login_msg)
            
            # 接收响应
            response = self.receive_message()
            if response and response.get('type') == 'LOGIN_SUCCESS':
                # 保存用户信息
                self.user_id = response['content']['user_id']
                self.nickname = response['content']['nickname']
                self.username = username
                self.server_ip = server_ip
                self.connected = True
                
                # 保存好友列表
                for friend in response['content']['friends']:
                    self.friends[friend['user_id']] = friend
                
                # 保存未读消息
                self.unread_counts = response['content']['unread_messages']
                
                # 显示主界面
                self.show_main_screen()
                self.refresh_friend_list()
                
                # 显示未读消息提示
                self.show_unread_notifications()
                
            else:
                error_msg = response['content']['error'] if response else '登录失败'
                messagebox.showerror("登录失败", error_msg)
                self.sock.close()
                self.sock = None
                
        except Exception as e:
            messagebox.showerror("连接错误", f"无法连接到服务器: {str(e)}")
    
    def register(self):
        """注册新用户"""
        server_ip = self.server_var.get()
        username = self.user_var.get()
        password = self.pass_var.get()
        
        if not all([server_ip, username, password]):
            messagebox.showwarning("输入错误", "请填写完整信息")
            return
        
        # 弹出注册对话框
        register_dialog = tk.Toplevel(self.root)
        register_dialog.title("用户注册")
        register_dialog.geometry("300x200")
        register_dialog.resizable(False, False)
        
        tk.Label(register_dialog, text="昵称:", 
                font=("微软雅黑", 11)).pack(pady=10)
        
        nickname_var = tk.StringVar(value=username)
        tk.Entry(register_dialog, textvariable=nickname_var, 
                font=("微软雅黑", 11), width=20).pack()
        
        def do_register():
            nickname = nickname_var.get()
            
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock.connect((server_ip, self.server_port))
                
                register_msg = {
                    'protocol': 'LCP/1.0',
                    'type': 'REGISTER',
                    'timestamp': datetime.now().isoformat(),
                    'content': {
                        'username': username,
                        'password': password,
                        'nickname': nickname
                    }
                }
                
                self.send_message_to_socket(sock, register_msg)
                
                response = self.receive_message_from_socket(sock)
                sock.close()
                
                if response and response.get('type') == 'REGISTER_SUCCESS':
                    messagebox.showinfo("注册成功", "注册成功，请登录")
                    register_dialog.destroy()
                else:
                    error_msg = response['content']['error'] if response else '注册失败'
                    messagebox.showerror("注册失败", error_msg)
                    
            except Exception as e:
                messagebox.showerror("连接错误", f"无法连接到服务器: {str(e)}")
        
        tk.Button(register_dialog, text="注册", command=do_register,
                 font=("微软雅黑", 11), bg="#2196F3", fg="white").pack(pady=20)
        
        register_dialog.transient(self.root)
        register_dialog.grab_set()
        self.root.wait_window(register_dialog)
    
    def start_lan_discovery(self):
        """启动局域网发现"""
        try:
            # 创建UDP套接字用于发现
            self.discovery_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            self.discovery_sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
            self.discovery_sock.settimeout(1)
            
            # 启动发现线程
            discovery_thread = threading.Thread(target=self.discovery_worker, daemon=True)
            discovery_thread.start()
            
        except Exception as e:
            print(f"局域网发现启动失败: {str(e)}")
    
    def discovery_worker(self):
        """局域网发现工作线程"""
        while True:
            try:
                # 发送发现广播
                broadcast_msg = {
                    'protocol': 'LCP/1.0',
                    'type': 'DISCOVER',
                    'timestamp': datetime.now().isoformat()
                }
                
                self.discovery_sock.sendto(
                    json.dumps(broadcast_msg).encode('utf-8'),
                    ('255.255.255.255', self.broadcast_port)
                )
                
                # 接收响应
                try:
                    data, addr = self.discovery_sock.recvfrom(1024)
                    response = json.loads(data.decode('utf-8'))
                    
                    if response.get('type') == 'DISCOVER_RESPONSE':
                        # 添加到局域网用户列表
                        user_info = {
                            'ip': addr[0],
                            'port': response.get('server_port'),
                            'server_ip': response.get('server_ip')
                        }
                        
                        # 避免重复添加
                        if not any(u['ip'] == addr[0] for u in self.lan_users):
                            self.lan_users.append(user_info)
                            
                            # 更新UI
                            if hasattr(self, 'lan_listbox'):
                                self.root.after(0, self.update_lan_list)
                            
                except socket.timeout:
                    pass
                
                time.sleep(5)  # 每5秒发现一次
                
            except Exception as e:
                print(f"局域网发现错误: {str(e)}")
                time.sleep(10)
    
    def start_heartbeat(self):
        """启动心跳"""
        if not self.user_id:
            return
        
        try:
            self.heartbeat_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            
            # 心跳线程
            heartbeat_thread = threading.Thread(target=self.heartbeat_worker, daemon=True)
            heartbeat_thread.start()
            
        except Exception as e:
            print(f"心跳启动失败: {str(e)}")
    
    def heartbeat_worker(self):
        """心跳工作线程"""
        while self.connected:
            try:
                heartbeat_msg = {
                    'protocol': 'LCP/1.0',
                    'type': 'HEARTBEAT',
                    'from': self.user_id,
                    'timestamp': datetime.now().isoformat()
                }
                
                self.heartbeat_sock.sendto(
                    json.dumps(heartbeat_msg).encode('utf-8'),
                    (self.server_ip, self.heartbeat_port)
                )
                
                time.sleep(30)  # 每30秒发送一次心跳
                
            except Exception as e:
                print(f"心跳发送失败: {str(e)}")
                time.sleep(10)
    
    def start_receive_thread(self):
        """启动消息接收线程"""
        receive_thread = threading.Thread(target=self.receive_worker, daemon=True)
        receive_thread.start()
    
    def receive_worker(self):
        """消息接收工作线程"""
        while self.connected and self.sock:
            try:
                message = self.receive_message()
                if message:
                    self.handle_server_message(message)
                    
            except Exception as e:
                print(f"接收消息错误: {str(e)}")
                time.sleep(1)
    
    def handle_server_message(self, message):
        """处理服务器消息"""
        msg_type = message.get('type')
        
        if msg_type == 'TEXT_MSG':
            self.handle_text_message(message)
        elif msg_type == 'IMAGE_MSG':
            self.handle_image_message(message)
        elif msg_type == 'FRIEND_REQUEST':
            self.handle_friend_request(message)
        elif msg_type == 'FRIEND_ADDED':
            self.handle_friend_added(message)
        elif msg_type == 'FRIEND_DELETED':
            self.handle_friend_deleted(message)
        elif msg_type == 'ONLINE_STATUS':
            self.handle_online_status(message)
        elif msg_type == 'ERROR':
            self.handle_error_message(message)
    
    def handle_text_message(self, message):
        """处理收到的文本消息"""
        from_user = message.get('from')
        text = message.get('content', {}).get('text', '')
        timestamp = message.get('content', {}).get('timestamp', '')
        
        if not from_user:
            return
        
        # 添加到消息历史
        if from_user not in self.messages:
            self.messages[from_user] = []
        
        self.messages[from_user].append({
            'type': 'text',
            'from': from_user,
            'text': text,
            'timestamp': timestamp,
            'is_me': False
        })
        
        # 更新未读计数
        self.unread_counts[from_user] = self.unread_counts.get(from_user, 0) + 1
        
        # 如果当前正在和该用户聊天，显示消息
        if self.current_chat == from_user:
            self.display_message(from_user, text, False, timestamp)
            # 发送已读回执
            self.send_read_receipt(from_user)
        else:
            # 显示通知
            friend_name = self.get_friend_name(from_user)
            self.show_notification(f"新消息来自 {friend_name}", text[:50])
            # 更新好友列表显示
            self.update_friend_unread(from_user)
    
    def send_message(self):
        """发送消息"""
        if not self.current_chat:
            messagebox.showwarning("未选择", "请选择聊天对象")
            return
        
        text = self.input_text.get("1.0", tk.END).strip()
        if not text:
            return
        
        # 构建消息
        message = {
            'protocol': 'LCP/1.0',
            'type': 'TEXT_MSG',
            'from': self.user_id,
            'to': self.current_chat,
            'timestamp': datetime.now().isoformat(),
            'content': {
                'text': text
            }
        }
        
        try:
            # 发送消息
            self.send_message_to_server(message)
            
            # 添加到本地消息历史
            if self.current_chat not in self.messages:
                self.messages[self.current_chat] = []
            
            self.messages[self.current_chat].append({
                'type': 'text',
                'from': self.user_id,
                'text': text,
                'timestamp': datetime.now().isoformat(),
                'is_me': True
            })
            
            # 显示消息
            self.display_message(self.current_chat, text, True)
            
            # 清空输入框
            self.input_text.delete("1.0", tk.END)
            
        except Exception as e:
            messagebox.showerror("发送失败", f"消息发送失败: {str(e)}")
    
    def display_message(self, user_id, text, is_me, timestamp=None):
        """在消息列表中显示消息"""
        if not timestamp:
            timestamp = datetime.now().strftime("%H:%M")
        
        # 格式化消息
        sender = "我" if is_me else self.get_friend_name(user_id)
        formatted_msg = f"[{timestamp}] {sender}: {text}"
        
        # 添加到消息列表
        self.message_list.insert(tk.END, formatted_msg)
        self.message_list.see(tk.END)
        
        # 根据发送者设置颜色
        if is_me:
            self.message_list.itemconfig(tk.END, {'fg': 'blue'})
        else:
            self.message_list.itemconfig(tk.END, {'fg': 'green'})
    
    def on_friend_double_click(self, event):
        """双击好友列表项"""
        selection = self.friend_tree.selection()
        if selection:
            item = self.friend_tree.item(selection[0])
            user_id = item.get('values', [''])[0]
            
            if user_id:
                self.open_chat(user_id)
    
    def open_chat(self, user_id):
        """打开聊天窗口"""
        self.current_chat = user_id
        
        # 更新标题
        friend_name = self.get_friend_name(user_id)
        online_status = "● 在线" if self.friends.get(user_id, {}).get('is_online') else "○ 离线"
        self.chat_title.config(text=f"{friend_name}  {online_status}")
        
        # 清空消息列表
        self.message_list.delete(0, tk.END)
        
        # 加载消息历史
        if user_id in self.messages:
            for msg in self.messages[user_id]:
                if msg['type'] == 'text':
                    self.display_message(user_id, msg['text'], msg['is_me'], msg['timestamp'])
        
        # 重置未读计数
        self.unread_counts[user_id] = 0
        self.update_friend_unread(user_id)
        
        # 发送已读回执
        self.send_read_receipt(user_id)
    
    def send_read_receipt(self, user_id):
        """发送已读回执"""
        if not self.connected:
            return
        
        receipt = {
            'protocol': 'LCP/1.0',
            'type': 'READ_RECEIPT',
            'from': self.user_id,
            'to': user_id,
            'timestamp': datetime.now().isoformat()
        }
        
        try:
            self.send_message_to_server(receipt)
        except:
            pass
    
    def show_add_friend(self):
        """显示添加好友对话框"""
        dialog = tk.Toplevel(self.root)
        dialog.title("添加好友")
        dialog.geometry("400x300")
        dialog.resizable(False, False)
        
        # 搜索框
        search_frame = tk.Frame(dialog)
        search_frame.pack(pady=20, padx=20, fill='x')
        
        tk.Label(search_frame, text="搜索用户:", 
                font=("微软雅黑", 11)).pack(anchor='w')
        
        search_var = tk.StringVar()
        tk.Entry(search_frame, textvariable=search_var, 
                font=("微软雅黑", 11)).pack(fill='x', pady=5)
        
        # 搜索结果
        result_frame = tk.Frame(dialog)
        result_frame.pack(pady=10, padx=20, fill='both', expand=True)
        
        result_listbox = tk.Listbox(result_frame, font=("微软雅黑", 10))
        scrollbar = tk.Scrollbar(result_frame, command=result_listbox.yview)
        result_listbox.config(yscrollcommand=scrollbar.set)
        
        result_listbox.pack(side='left', fill='both', expand=True)
        scrollbar.pack(side='right', fill='y')
        
        # 验证消息
        msg_frame = tk.Frame(dialog)
        msg_frame.pack(pady=10, padx=20, fill='x')
        
        tk.Label(msg_frame, text="验证消息:", 
                font=("微软雅黑", 11)).pack(anchor='w')
        
        msg_var = tk.StringVar(value="我是" + (self.nickname or self.username))
        tk.Entry(msg_frame, textvariable=msg_var, 
                font=("微软雅黑", 11)).pack(fill='x', pady=5)
        
        def do_search():
            keyword = search_var.get().strip()
            if not keyword:
                messagebox.showwarning("输入错误", "请输入搜索关键词")
                return
            
            # 发送搜索请求
            search_msg = {
                'protocol': 'LCP/1.0',
                'type': 'SEARCH_USER',
                'from': self.user_id,
                'timestamp': datetime.now().isoformat(),
                'content': {
                    'keyword': keyword,
                    'type': 'username'
                }
            }
            
            try:
                self.send_message_to_server(search_msg)
            except Exception as e:
                messagebox.showerror("搜索失败", str(e))
        
        def do_add_friend():
            selection = result_listbox.curselection()
            if not selection:
                messagebox.showwarning("未选择", "请选择要添加的用户")
                return
            
            index = selection[0]
            user_id = search_results[index]['user_id']
            message = msg_var.get()
            
            # 发送好友请求
            add_msg = {
                'protocol': 'LCP/1.0',
                'type': 'ADD_FRIEND',
                'from': self.user_id,
                'timestamp': datetime.now().isoformat(),
                'content': {
                    'user_id': user_id,
                    'message': message
                }
            }
            
            try:
                self.send_message_to_server(add_msg)
                messagebox.showinfo("成功", "好友请求已发送")
                dialog.destroy()
            except Exception as e:
                messagebox.showerror("发送失败", str(e))
        
        # 按钮
        button_frame = tk.Frame(dialog)
        button_frame.pack(pady=20)
        
        tk.Button(button_frame, text="搜索", command=do_search,
                 font=("微软雅黑", 11)).pack(side='left', padx=10)
        tk.Button(button_frame, text="添加好友", command=do_add_friend,
                 font=("微软雅黑", 11), bg="#4CAF50", fg="white").pack(side='left', padx=10)
        
        # 用于存储搜索结果
        search_results = []
        
        def update_result_list(results):
            result_listbox.delete(0, tk.END)
            search_results.clear()
            
            for user in results:
                display_text = f"{user['nickname']} ({user['username']})"
                if user['is_friend']:
                    display_text += " [已是好友]"
                elif user['request_status'] == 0:
                    display_text += " [请求待处理]"
                elif user['request_status'] == 1:
                    display_text += " [请求已接受]"
                
                result_listbox.insert(tk.END, display_text)
                search_results.append(user)
        
        dialog.transient(self.root)
        dialog.grab_set()
        
        # 绑定搜索事件
        search_var.trace('w', lambda *args: do_search() if search_var.get() else None)
        
        self.root.wait_window(dialog)
    
    def send_image(self):
        """发送图片"""
        if not self.current_chat:
            messagebox.showwarning("未选择", "请选择聊天对象")
            return
        
        # 选择图片文件
        file_path = filedialog.askopenfilename(
            title="选择图片",
            filetypes=[("Image files", "*.jpg *.jpeg *.png *.gif *.bmp"), 
                      ("All files", "*.*")]
        )
        
        if not file_path:
            return
        
        try:
            # 读取图片并编码为base64
            with open(file_path, 'rb') as f:
                image_data = base64.b64encode(f.read()).decode('utf-8')
            
            file_name = os.path.basename(file_path)
            
            # 构建消息
            message = {
                'protocol': 'LCP/1.0',
                'type': 'IMAGE_MSG',
                'from': self.user_id,
                'to': self.current_chat,
                'timestamp': datetime.now().isoformat(),
                'content': {
                    'image_data': image_data,
                    'image_name': file_name
                }
            }
            
            # 发送消息
            self.send_message_to_server(message)
            
            # 添加到本地消息历史
            if self.current_chat not in self.messages:
                self.messages[self.current_chat] = []
            
            self.messages[self.current_chat].append({
                'type': 'image',
                'from': self.user_id,
                'image_name': file_name,
                'timestamp': datetime.now().isoformat(),
                'is_me': True
            })
            
            # 显示消息
            self.display_message(self.current_chat, f"[图片] {file_name}", True)
            
        except Exception as e:
            messagebox.showerror("发送失败", f"图片发送失败: {str(e)}")
    
    def send_file(self):
        """发送文件"""
        if not self.current_chat:
            messagebox.showwarning("未选择", "请选择聊天对象")
            return
        
        # 选择文件
        file_path = filedialog.askopenfilename(
            title="选择文件",
            filetypes=[("All files", "*.*")]
        )
        
        if not file_path:
            return
        
        try:
            # 连接文件服务器
            file_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            file_sock.connect((self.server_ip, self.file_port))
            
            # 读取文件
            with open(file_path, 'rb') as f:
                file_data = f.read()
            
            file_name = os.path.basename(file_path)
            file_size = len(file_data)
            
            # 发送文件头
            header = f"File-Name: {file_name}\r\n"
            header += f"File-Size: {file_size}\r\n"
            header += f"File-Type: file\r\n"
            header += f"User-ID: {self.user_id}\r\n"
            header += f"To-User: {self.current_chat}\r\n"
            header += "\r\n"
            
            file_sock.send(header.encode('utf-8'))
            
            # 发送文件数据
            file_sock.sendall(file_data)
            
            # 接收响应
            response = self.receive_message_from_socket(file_sock)
            file_sock.close()
            
            if response and response.get('type') == 'FILE_UPLOAD_SUCCESS':
                # 添加到本地消息历史
                if self.current_chat not in self.messages:
                    self.messages[self.current_chat] = []
                
                self.messages[self.current_chat].append({
                    'type': 'file',
                    'from': self.user_id,
                    'file_name': file_name,
                    'timestamp': datetime.now().isoformat(),
                    'is_me': True
                })
                
                # 显示消息
                self.display_message(self.current_chat, f"[文件] {file_name}", True)
            else:
                messagebox.showerror("发送失败", "文件发送失败")
            
        except Exception as e:
            messagebox.showerror("发送失败", f"文件发送失败: {str(e)}")
    
    def logout(self):
        """退出登录"""
        if self.connected:
            # 发送登出消息
            logout_msg = {
                'protocol': 'LCP/1.0',
                'type': 'LOGOUT',
                'from': self.user_id,
                'timestamp': datetime.now().isoformat()
            }
            
            try:
                self.send_message_to_server(logout_msg)
            except:
                pass
            
            self.sock.close()
            self.connected = False
        
        self.root.quit()
    
    # 辅助方法
    def send_message_to_server(self, message):
        """发送消息到服务器"""
        self.send_message_to_socket(self.sock, message)
    
    def send_message_to_socket(self, sock, message):
        """发送消息到指定socket"""
        if isinstance(message, dict):
            message = json.dumps(message, ensure_ascii=False)
        
        message_bytes = message.encode('utf-8')
        length = len(message_bytes)
        
        sock.send(struct.pack('!I', length))
        sock.send(message_bytes)
    
    def receive_message(self):
        """从服务器接收消息"""
        return self.receive_message_from_socket(self.sock)
    
    def receive_message_from_socket(self, sock):
        """从指定socket接收消息"""
        try:
            # 读取长度
            length_data = sock.recv(4)
            if len(length_data) < 4:
                return None
            
            length = struct.unpack('!I', length_data)[0]
            
            # 读取消息
            chunks = []
            received = 0
            
            while received < length:
                chunk = sock.recv(min(length - received, 4096))
                if not chunk:
                    break
                chunks.append(chunk)
                received += len(chunk)
            
            if received < length:
                return None
            
            return json.loads(b''.join(chunks).decode('utf-8'))
            
        except:
            return None
    
    def get_friend_name(self, user_id):
        """获取好友名称"""
        friend = self.friends.get(user_id, {})
        return friend.get('nickname', friend.get('username', user_id))
    
    def refresh_friend_list(self):
        """刷新好友列表"""
        # 清空列表
        for item in self.friend_tree.get_children():
            self.friend_tree.delete(item)
        
        # 按字母排序
        sorted_friends = sorted(self.friends.values(), key=lambda x: x['nickname'].upper())
        
        # 添加好友到列表
        for friend in sorted_friends:
            # 状态图标
            status_icon = "●" if friend.get('is_online') else "○"
            status_color = "green" if friend.get('is_online') else "gray"
            
            # 未读消息数
            unread = self.unread_counts.get(friend['user_id'], 0)
            unread_text = str(unread) if unread > 0 else ""
            
            # 添加项目
            item = self.friend_tree.insert('', 'end', values=(
                friend['user_id'],  # 隐藏列，存储用户ID
                f"{friend['nickname']}",
                unread_text
            ))
            
            # 设置状态图标（需要自定义图标，这里用文字代替）
            self.friend_tree.set(item, 'status', status_icon)
    
    def update_friend_unread(self, user_id):
        """更新好友未读消息显示"""
        for item in self.friend_tree.get_children():
            values = self.friend_tree.item(item, 'values')
            if values and values[0] == user_id:
                unread = self.unread_counts.get(user_id, 0)
                unread_text = str(unread) if unread > 0 else ""
                self.friend_tree.set(item, 'unread', unread_text)
                break
    
    def update_lan_list(self):
        """更新局域网用户列表"""
        if hasattr(self, 'lan_listbox'):
            self.lan_listbox.delete(0, tk.END)
            for user in self.lan_users:
                self.lan_listbox.insert(tk.END, f"{user['ip']}:{user['port']}")
    
    def clear_window(self):
        """清空窗口内容"""
        for widget in self.root.winfo_children():
            widget.destroy()
    
    def on_enter_pressed(self, event):
        """处理回车键"""
        if event.state & 0x1:  # Shift键被按下
            return  # 允许换行
        else:
            self.send_message()
            return 'break'  # 阻止默认行为
    
    def filter_friends(self, event):
        """过滤好友列表"""
        keyword = self.search_var.get().lower()
        
        for item in self.friend_tree.get_children():
            values = self.friend_tree.item(item, 'values')
            friend_name = values[1].lower() if len(values) > 1 else ""
            
            if keyword in friend_name:
                self.friend_tree.item(item, tags=('visible',))
            else:
                self.friend_tree.item(item, tags=('hidden',))
        
        # 隐藏不匹配的项目
        self.friend_tree.tag_configure('hidden', visible=False)
    
    def show_notification(self, title, message):
        """显示通知"""
        # 可以在这里实现系统通知
        print(f"通知: {title} - {message}")
    
    def show_unread_notifications(self):
        """显示未读消息通知"""
        for user_id, count in self.unread_counts.items():
            if count > 0:
                friend_name = self.get_friend_name(user_id)
                self.show_notification(f"未读消息 ({count})", f"来自 {friend_name}")

def main():
    root = tk.Tk()
    app = ChatClientGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()