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