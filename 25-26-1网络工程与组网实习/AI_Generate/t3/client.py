import tkinter as tk
from tkinter import ttk, messagebox, filedialog, scrolledtext, font
import socket
import json
import threading
import os
from datetime import datetime

class NovelReaderGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("小说阅读器 - NRP客户端")
        self.root.geometry("1200x800")
        
        # 连接状态
        self.connected = False
        self.sock = None
        self.server_host = 'localhost'
        self.server_port = 8889
        
        # 当前阅读状态
        self.current_novel = None
        self.current_page = 1
        self.total_pages = 1
        self.page_size = 2000
        self.bookmarks = []
        
        # 字体设置
        self.font_family = "Microsoft YaHei"
        self.font_size = 12
        
        self.setup_ui()
        
        # 尝试连接服务器
        self.connect_server()
    
    def setup_ui(self):
        """设置用户界面"""
        # 创建菜单栏
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)
        
        # 文件菜单
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="文件", menu=file_menu)
        file_menu.add_command(label="打开小说", command=self.open_novel_dialog)
        file_menu.add_command(label="重新连接", command=self.reconnect_server)
        file_menu.add_separator()
        file_menu.add_command(label="退出", command=self.root.quit)
        
        # 视图菜单
        view_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="视图", menu=view_menu)
        view_menu.add_command(label="增大字体", command=lambda: self.change_font_size(1))
        view_menu.add_command(label="减小字体", command=lambda: self.change_font_size(-1))
        view_menu.add_separator()
        view_menu.add_command(label="白天模式", command=lambda: self.change_theme('light'))
        view_menu.add_command(label="夜间模式", command=lambda: self.change_theme('dark'))
        
        # 书签菜单
        bookmark_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="书签", menu=bookmark_menu)
        bookmark_menu.add_command(label="添加书签", command=self.add_bookmark)
        bookmark_menu.add_command(label="管理书签", command=self.manage_bookmarks)
        
        # 创建主框架
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill='both', expand=True, padx=10, pady=10)
        
        # 左侧边栏（小说列表）
        sidebar = ttk.Frame(main_frame, width=250)
        sidebar.pack(side='left', fill='y', padx=(0, 10))
        
        # 小说列表标题
        ttk.Label(sidebar, text="小说库", font=('Microsoft YaHei', 14, 'bold')).pack(pady=10)
        
        # 小说列表
        list_frame = ttk.Frame(sidebar)
        list_frame.pack(fill='both', expand=True)
        
        # 创建Treeview显示小说列表
        columns = ('标题', '大小')
        self.novel_tree = ttk.Treeview(list_frame, columns=columns, show='tree', height=20)
        
        # 设置列
        self.novel_tree.column('#0', width=200, minwidth=200)
        self.novel_tree.column('大小', width=80, minwidth=80, anchor='e')
        
        self.novel_tree.heading('#0', text='标题')
        self.novel_tree.heading('大小', text='大小')
        
        # 滚动条
        scrollbar = ttk.Scrollbar(list_frame, orient='vertical', command=self.novel_tree.yview)
        self.novel_tree.configure(yscrollcommand=scrollbar.set)
        
        self.novel_tree.pack(side='left', fill='both', expand=True)
        scrollbar.pack(side='right', fill='y')
        
        # 绑定选择事件
        self.novel_tree.bind('<<TreeviewSelect>>', self.on_novel_selected)
        
        # 刷新按钮
        ttk.Button(sidebar, text="刷新列表", command=self.refresh_novel_list).pack(pady=10)
        
        # 右侧主阅读区
        right_frame = ttk.Frame(main_frame)
        right_frame.pack(side='right', fill='both', expand=True)
        
        # 顶部工具栏
        toolbar = ttk.Frame(right_frame)
        toolbar.pack(fill='x', pady=(0, 10))
        
        # 导航按钮
        nav_frame = ttk.Frame(toolbar)
        nav_frame.pack(side='left')
        
        ttk.Button(nav_frame, text="首页", 
                  command=lambda: self.go_to_page(1)).pack(side='left', padx=2)
        ttk.Button(nav_frame, text="上一页", 
                  command=self.prev_page).pack(side='left', padx=2)
        
        # 页码输入
        self.page_var = tk.StringVar(value="1")
        page_entry = ttk.Entry(nav_frame, textvariable=self.page_var, width=6)
        page_entry.pack(side='left', padx=2)
        ttk.Label(nav_frame, text=f"/ {self.total_pages}").pack(side='left', padx=2)
        
        ttk.Button(nav_frame, text="跳转", 
                  command=self.jump_to_page).pack(side='left', padx=2)
        ttk.Button(nav_frame, text="下一页", 
                  command=self.next_page).pack(side='left', padx=2)
        ttk.Button(nav_frame, text="末页", 
                  command=self.last_page).pack(side='left', padx=2)
        
        # 章节导航
        ttk.Button(toolbar, text="上一章", 
                  command=self.prev_chapter).pack(side='left', padx=5)
        ttk.Button(toolbar, text="下一章", 
                  command=self.next_chapter).pack(side='left', padx=5)
        
        # 搜索框
        search_frame = ttk.Frame(toolbar)
        search_frame.pack(side='right')
        
        ttk.Label(search_frame, text="搜索:").pack(side='left')
        self.search_var = tk.StringVar()
        search_entry = ttk.Entry(search_frame, textvariable=self.search_var, width=20)
        search_entry.pack(side='left', padx=5)
        ttk.Button(search_frame, text="搜索", 
                  command=self.search_content).pack(side='left')
        
        # 阅读区域
        text_frame = ttk.Frame(right_frame)
        text_frame.pack(fill='both', expand=True)
        
        # 创建带样式的Text组件
        self.text_area = tk.Text(
            text_frame,
            wrap='word',
            font=(self.font_family, self.font_size),
            bg='white',
            fg='black',
            padx=20,
            pady=20,
            spacing1=5,  # 段落前间距
            spacing2=2,  # 行间距
            spacing3=5,  # 段落后间距
            state='disabled'
        )
        
        # 创建滚动条
        text_scrollbar = ttk.Scrollbar(text_frame, orient='vertical', command=self.text_area.yview)
        self.text_area.configure(yscrollcommand=text_scrollbar.set)
        
        self.text_area.pack(side='left', fill='both', expand=True)
        text_scrollbar.pack(side='right', fill='y')
        
        # 底部状态栏
        status_frame = ttk.Frame(right_frame)
        status_frame.pack(fill='x', pady=(10, 0))
        
        # 状态信息
        self.status_var = tk.StringVar(value="就绪")
        ttk.Label(status_frame, textvariable=self.status_var).pack(side='left')
        
        # 进度信息
        self.progress_var = tk.StringVar(value="")
        ttk.Label(status_frame, textvariable=self.progress_var).pack(side='right')
        
        # 绑定事件
        self.root.bind('<Left>', lambda e: self.prev_page())
        self.root.bind('<Right>', lambda e: self.next_page())
        self.root.bind('<Control-f>', lambda e: search_entry.focus())
        self.root.bind('<Control-s>', lambda e: self.search_content())
        self.root.bind('<Control-d>', lambda e: self.download_novel())
        
        # 初始刷新小说列表
        self.refresh_novel_list()
    
    def send_request(self, method, novel_id='', data=None):
        """发送请求到服务器"""
        if not self.connected:
            messagebox.showwarning("未连接", "请先连接到服务器")
            return None
        
        try:
            # 构建请求
            body = json.dumps(data) if data else ''
            
            request = f"NRP/1.0 {method} {novel_id}\r\n"
            request += f"Content-Type: application/json\r\n"
            request += f"Content-Length: {len(body.encode('utf-8'))}\r\n"
            request += f"\r\n"
            request += body
            
            self.sock.send(request.encode('utf-8'))
            
            # 接收响应
            response_data = b""
            while True:
                chunk = self.sock.recv(8192)
                if not chunk:
                    break
                response_data += chunk
                
                # 检查是否接收完成
                if b'\r\n\r\n' in response_data:
                    # 解析头部获取Content-Length
                    header_end = response_data.find(b'\r\n\r\n')
                    headers = response_data[:header_end].decode('utf-8')
                    
                    # 提取Content-Length
                    content_length = 0
                    for line in headers.split('\r\n'):
                        if line.lower().startswith('content-length:'):
                            content_length = int(line.split(':')[1].strip())
                            break
                    
                    # 检查是否接收完所有数据
                    if len(response_data) >= header_end + 4 + content_length:
                        break
            
            # 解析响应
            response_str = response_data.decode('utf-8')
            header_end = response_str.find('\r\n\r\n')
            headers_str = response_str[:header_end]
            body_str = response_str[header_end + 4:]
            
            # 解析状态行
            status_line = headers_str.split('\r\n')[0]
            parts = status_line.split()
            if len(parts) >= 3:
                _, status_code, status_text = parts[0], int(parts[1]), ' '.join(parts[2:])
            else:
                status_code, status_text = 500, "Invalid Response"
            
            if status_code == 200 or status_code == 201:
                try:
                    return json.loads(body_str)
                except:
                    return body_str
            else:
                messagebox.showerror("错误", f"{status_text}")
                return None
                
        except Exception as e:
            messagebox.showerror("通信错误", f"无法连接到服务器: {str(e)}")
            self.connected = False
            return None
    
    def connect_server(self):
        """连接服务器"""
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.settimeout(5)
            self.sock.connect((self.server_host, self.server_port))
            self.connected = True
            self.status_var.set(f"已连接到服务器 {self.server_host}:{self.server_port}")
        except Exception as e:
            self.connected = False
            self.status_var.set("无法连接到服务器")
            messagebox.showwarning("连接失败", 
                                 f"无法连接到服务器 {self.server_host}:{self.server_port}\n"
                                 f"请确保服务器已启动。")
    
    def reconnect_server(self):
        """重新连接服务器"""
        if self.sock:
            try:
                self.sock.close()
            except:
                pass
        
        self.connect_server()
        if self.connected:
            self.refresh_novel_list()
    
    def refresh_novel_list(self):
        """刷新小说列表"""
        if not self.connected:
            return
        
        # 清空列表
        for item in self.novel_tree.get_children():
            self.novel_tree.delete(item)
        
        # 获取小说列表
        data = self.send_request('LIST', '')
        if data and 'novels' in data:
            novels = data['novels']
            for novel in novels:
                novel_id = novel['id']
                title = novel['title']
                size = self.format_size(novel['size'])
                
                item_id = self.novel_tree.insert('', 'end', text=title, values=(size,))
                self.novel_tree.set(item_id, '大小', size)
                
                # 存储小说ID
                self.novel_tree.item(item_id, tags=(novel_id,))
    
    def on_novel_selected(self, event):
        """选择小说"""
        selection = self.novel_tree.selection()
        if not selection:
            return
        
        item = selection[0]
        novel_id = self.novel_tree.item(item, 'tags')[0]
        
        # 打开小说
        self.open_novel(novel_id)
    
    def open_novel_dialog(self):
        """打开本地小说对话框"""
        if not self.connected:
            messagebox.showwarning("未连接", "请先连接到服务器")
            return
        
        file_path = filedialog.askopenfilename(
            title="选择小说文件",
            filetypes=[("文本文件", "*.txt"), ("所有文件", "*.*")]
        )
        
        if file_path:
            # 这里应该实现上传功能，但为了简化，我们先加载到本地阅读
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                # 创建临时显示
                self.display_content(content[:2000])
                self.current_novel = os.path.basename(file_path)
                self.status_var.set(f"正在阅读: {self.current_novel}")
                
            except Exception as e:
                messagebox.showerror("打开失败", f"无法打开文件: {str(e)}")
    
    def open_novel(self, novel_id):
        """打开服务器上的小说"""
        if not self.connected:
            return
        
        # 发送打开请求
        data = {
            'page_size': self.page_size,
            'encoding': 'utf-8'
        }
        
        response = self.send_request('OPEN', novel_id, data)
        if response:
            self.current_novel = novel_id
            self.current_page = 1
            self.total_pages = response.get('total_pages', 1)
            
            # 加载第一页
            self.load_page(1)
            
            # 更新状态
            self.status_var.set(f"正在阅读: {response.get('title', '未知')}")
            self.update_progress()
            
            # 加载书签
            self.load_bookmarks()
    
    def load_page(self, page):
        """加载指定页"""
        if not self.connected or not self.current_novel:
            return
        
        data = {
            'page': page,
            'page_size': self.page_size
        }
        
        response = self.send_request('PAGE', self.current_novel, data)
        if response:
            self.current_page = page
            self.total_pages = response['total_pages']
            
            # 显示内容
            self.display_content(response['content'])
            
            # 更新页码显示
            self.page_var.set(str(page))
            
            # 更新进度
            self.update_progress()
    
    def display_content(self, content):
        """在文本区域显示内容"""
        self.text_area.config(state='normal')
        self.text_area.delete(1.0, tk.END)
        
        # 处理章节标题
        import re
        
        # 查找章节标题并格式化
        lines = content.split('\n')
        formatted_lines = []
        
        for line in lines:
            # 检查是否为章节标题
            if re.match(r'^第[零一二三四五六七八九十百千\d]+[章节回]\s*.+$', line.strip()):
                formatted_lines.append(f"\n\n【{line.strip()}】\n")
            else:
                formatted_lines.append(line)
        
        formatted_content = '\n'.join(formatted_lines)
        
        # 插入文本
        self.text_area.insert(1.0, formatted_content)
        
        # 配置标签（用于高亮等）
        self.text_area.tag_configure('chapter', font=(self.font_family, self.font_size + 2, 'bold'), 
                                   foreground='darkblue', spacing3=10)
        
        # 应用章节样式
        start_idx = '1.0'
        while True:
            start_idx = self.text_area.search('【', start_idx, stopindex=tk.END)
            if not start_idx:
                break
            
            end_idx = self.text_area.search('】', start_idx, stopindex=tk.END)
            if not end_idx:
                break
            
            end_idx = f"{end_idx}+1c"  # 包含】字符
            
            # 应用标签
            self.text_area.tag_add('chapter', start_idx, end_idx)
            
            start_idx = end_idx
        
        self.text_area.config(state='disabled')
        self.text_area.see('1.0')  # 滚动到顶部
    
    def prev_page(self):
        """上一页"""
        if self.current_page > 1:
            self.load_page(self.current_page - 1)
    
    def next_page(self):
        """下一页"""
        if self.current_page < self.total_pages:
            self.load_page(self.current_page + 1)
    
    def go_to_page(self, page):
        """跳转到指定页"""
        if 1 <= page <= self.total_pages:
            self.load_page(page)
    
    def jump_to_page(self):
        """跳转到输入页"""
        try:
            page = int(self.page_var.get())
            self.go_to_page(page)
        except ValueError:
            messagebox.showwarning("输入错误", "请输入有效的页码")
    
    def first_page(self):
        """首页"""
        self.go_to_page(1)
    
    def last_page(self):
        """末页"""
        self.go_to_page(self.total_pages)
    
    def prev_chapter(self):
        """上一章"""
        if not self.connected or not self.current_novel:
            return
        
        # 获取小说信息
        info = self.send_request('INFO', self.current_novel)
        if info and 'chapter_positions' in info:
            chapters = info['chapter_positions']
            
            # 找到当前位置所在的章节
            current_pos = (self.current_page - 1) * self.page_size
            current_chapter = -1
            
            for i, chapter in enumerate(chapters):
                if chapter['position'] <= current_pos:
                    current_chapter = i
                else:
                    break
            
            # 跳转到上一章
            if current_chapter > 0:
                data = {'chapter': current_chapter - 1}
                response = self.send_request('CHAPTER', self.current_novel, data)
                if response:
                    self.current_page = response['page']
                    self.display_content(response['content'])
                    self.update_progress()
    
    def next_chapter(self):
        """下一章"""
        if not self.connected or not self.current_novel:
            return
        
        # 获取小说信息
        info = self.send_request('INFO', self.current_novel)
        if info and 'chapter_positions' in info:
            chapters = info['chapter_positions']
            
            # 找到当前位置所在的章节
            current_pos = (self.current_page - 1) * self.page_size
            current_chapter = -1
            
            for i, chapter in enumerate(chapters):
                if chapter['position'] <= current_pos:
                    current_chapter = i
                else:
                    break
            
            # 跳转到下一章
            if current_chapter + 1 < len(chapters):
                data = {'chapter': current_chapter + 1}
                response = self.send_request('CHAPTER', self.current_novel, data)
                if response:
                    self.current_page = response['page']
                    self.display_content(response['content'])
                    self.update_progress()
    
    def search_content(self):
        """搜索内容"""
        if not self.connected or not self.current_novel:
            return
        
        keyword = self.search_var.get().strip()
        if not keyword:
            messagebox.showwarning("搜索提示", "请输入搜索关键词")
            return
        
        # 创建搜索对话框
        search_window = tk.Toplevel(self.root)
        search_window.title(f"搜索: {keyword}")
        search_window.geometry("800x600")
        
        # 搜索框
        search_frame = ttk.Frame(search_window, padding=10)
        search_frame.pack(fill='x')
        
        ttk.Label(search_frame, text="关键词:").pack(side='left')
        keyword_var = tk.StringVar(value=keyword)
        ttk.Entry(search_frame, textvariable=keyword_var, width=30).pack(side='left', padx=5)
        ttk.Button(search_frame, text="搜索", 
                  command=lambda: self.perform_search(keyword_var.get(), results_text, search_window)).pack(side='left')
        
        # 搜索结果区域
        results_frame = ttk.Frame(search_window)
        results_frame.pack(fill='both', expand=True, padx=10, pady=(0, 10))
        
        results_text = scrolledtext.ScrolledText(results_frame, wrap='word')
        results_text.pack(fill='both', expand=True)
        
        # 执行初始搜索
        self.perform_search(keyword, results_text, search_window)
    
    def perform_search(self, keyword, text_widget, window):
        """执行搜索"""
        if not self.connected or not self.current_novel:
            return
        
        data = {'keyword': keyword, 'page': 1}
        response = self.send_request('SEARCH', self.current_novel, data)
        
        if response:
            text_widget.delete(1.0, tk.END)
            
            total_results = response['total_results']
            results = response['results']
            
            text_widget.insert(tk.END, f"找到 {total_results} 个结果\n\n")
            
            for i, result in enumerate(results, 1):
                text_widget.insert(tk.END, f"结果 {i} (第{result['page']}页):\n")
                text_widget.insert(tk.END, f"{result['preview']}\n")
                text_widget.insert(tk.END, "-" * 80 + "\n\n")
            
            # 添加跳转功能
            text_widget.tag_configure('page_link', foreground='blue', underline=True)
            
            # 查找所有页码并添加链接
            content = text_widget.get(1.0, tk.END)
            import re
            
            for match in re.finditer(r'第(\d+)页', content):
                start = f"1.0+{match.start()}c"
                end = f"1.0+{match.end()}c"
                
                text_widget.tag_add('page_link', start, end)
                text_widget.tag_bind('page_link', '<Button-1>', 
                                   lambda e, page=int(match.group(1)): self.go_to_page_and_close(page, window))
    
    def go_to_page_and_close(self, page, window):
        """跳转到指定页并关闭窗口"""
        self.go_to_page(page)
        window.destroy()
    
    def add_bookmark(self):
        """添加书签"""
        if not self.connected or not self.current_novel:
            return
        
        # 输入书签备注
        note = tk.simpledialog.askstring("添加书签", 
                                       f"为第 {self.current_page} 页添加备注（可选）:",
                                       parent=self.root)
        
        data = {
            'action': 'add',
            'page': self.current_page,
            'note': note if note else f"第{self.current_page}页"
        }
        
        response = self.send_request('BOOKMARK', self.current_novel, data)
        if response:
            messagebox.showinfo("成功", "书签添加成功")
            self.load_bookmarks()
    
    def load_bookmarks(self):
        """加载书签"""
        if not self.connected or not self.current_novel:
            return
        
        data = {'action': 'get'}
        response = self.send_request('BOOKMARK', self.current_novel, data)
        if response and 'bookmarks' in response:
            self.bookmarks = response['bookmarks']
    
    def manage_bookmarks(self):
        """管理书签"""
        if not self.connected or not self.current_novel:
            return
        
        # 创建书签管理窗口
        bookmark_window = tk.Toplevel(self.root)
        bookmark_window.title("书签管理")
        bookmark_window.geometry("600x400")
        
        # 书签列表
        tree_frame = ttk.Frame(bookmark_window)
        tree_frame.pack(fill='both', expand=True, padx=10, pady=10)
        
        columns = ('页码', '备注', '时间')
        tree = ttk.Treeview(tree_frame, columns=columns, show='headings')
        
        for col in columns:
            tree.heading(col, text=col)
            if col == '页码':
                tree.column(col, width=80, anchor='center')
            elif col == '时间':
                tree.column(col, width=150)
            else:
                tree.column(col, width=200)
        
        # 添加书签数据
        for bookmark in self.bookmarks:
            tree.insert('', 'end', values=(
                bookmark['page'],
                bookmark['note'],
                bookmark['timestamp']
            ))
        
        scrollbar = ttk.Scrollbar(tree_frame, orient='vertical', command=tree.yview)
        tree.configure(yscrollcommand=scrollbar.set)
        
        tree.pack(side='left', fill='both', expand=True)
        scrollbar.pack(side='right', fill='y')
        
        # 按钮区域
        button_frame = ttk.Frame(bookmark_window)
        button_frame.pack(fill='x', padx=10, pady=(0, 10))
        
        ttk.Button(button_frame, text="跳转到书签", 
                  command=lambda: self.goto_selected_bookmark(tree, bookmark_window)).pack(side='left', padx=5)
        ttk.Button(button_frame, text="删除书签", 
                  command=lambda: self.delete_selected_bookmark(tree)).pack(side='left', padx=5)
        ttk.Button(button_frame, text="关闭", 
                  command=bookmark_window.destroy).pack(side='right', padx=5)
        
        # 绑定双击事件
        tree.bind('<Double-Button-1>', lambda e: self.goto_selected_bookmark(tree, bookmark_window))
    
    def goto_selected_bookmark(self, tree, window):
        """跳转到选中的书签"""
        selection = tree.selection()
        if not selection:
            return
        
        item = tree.item(selection[0])
        page = item['values'][0]
        
        self.go_to_page(page)
        window.destroy()
    
    def delete_selected_bookmark(self, tree):
        """删除选中的书签"""
        selection = tree.selection()
        if not selection:
            return
        
        item = tree.item(selection[0])
        page = item['values'][0]
        
        if messagebox.askyesno("确认删除", f"确定要删除第{page}页的书签吗？"):
            data = {
                'action': 'delete',
                'page': page
            }
            
            response = self.send_request('BOOKMARK', self.current_novel, data)
            if response:
                tree.delete(selection[0])
                self.load_bookmarks()
    
    def download_novel(self):
        """下载小说"""
        if not self.connected or not self.current_novel:
            return
        
        save_path = filedialog.asksaveasfilename(
            title="保存小说",
            defaultextension=".txt",
            filetypes=[("文本文件", "*.txt"), ("所有文件", "*.*")]
        )
        
        if save_path:
            try:
                # 获取小说内容
                response = self.send_request('DOWNLOAD', self.current_novel)
                if response:
                    with open(save_path, 'w', encoding='utf-8') as f:
                        f.write(response)
                    
                    messagebox.showinfo("成功", "小说下载完成")
            except Exception as e:
                messagebox.showerror("下载失败", f"无法下载小说: {str(e)}")
    
    def change_font_size(self, delta):
        """改变字体大小"""
        self.font_size = max(8, min(24, self.font_size + delta))
        self.text_area.config(font=(self.font_family, self.font_size))
    
    def change_theme(self, theme):
        """更换主题"""
        if theme == 'dark':
            self.text_area.config(bg='#2b2b2b', fg='#ffffff', insertbackground='white')
        else:
            self.text_area.config(bg='white', fg='black', insertbackground='black')
    
    def update_progress(self):
        """更新进度信息"""
        if self.total_pages > 0:
            percent = (self.current_page / self.total_pages) * 100
            self.progress_var.set(f"第 {self.current_page}/{self.total_pages} 页 ({percent:.1f}%)")
        else:
            self.progress_var.set("")
    
    def format_size(self, size):
        """格式化文件大小"""
        for unit in ['B', 'KB', 'MB']:
            if size < 1024.0:
                return f"{size:.1f} {unit}"
            size /= 1024.0
        return f"{size:.1f} GB"

def main():
    root = tk.Tk()
    app = NovelReaderGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()