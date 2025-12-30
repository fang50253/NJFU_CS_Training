import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import socket
import json
import base64
import os

class StudentClientGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("学生管理系统 - SHP客户端")
        self.root.geometry("800x600")
        
        # 服务器配置
        self.server_host = 'localhost'
        self.server_port = 8888
        
        self.setup_ui()
    
    def setup_ui(self):
        """设置用户界面"""
        # 创建选项卡
        tab_control = ttk.Notebook(self.root)
        
        # 1. 添加学生选项卡
        add_tab = ttk.Frame(tab_control)
        self.setup_add_tab(add_tab)
        tab_control.add(add_tab, text='添加学生')
        
        # 2. 查询学生选项卡
        query_tab = ttk.Frame(tab_control)
        self.setup_query_tab(query_tab)
        tab_control.add(query_tab, text='查询学生')
        
        # 3. 学生列表选项卡
        list_tab = ttk.Frame(tab_control)
        self.setup_list_tab(list_tab)
        tab_control.add(list_tab, text='学生列表')
        
        # 4. 删除学生选项卡
        delete_tab = ttk.Frame(tab_control)
        self.setup_delete_tab(delete_tab)
        tab_control.add(delete_tab, text='删除学生')
        
        tab_control.pack(expand=1, fill='both')
        
        # 状态栏
        self.status_var = tk.StringVar(value="就绪")
        status_bar = ttk.Label(self.root, textvariable=self.status_var, 
                              relief=tk.SUNKEN, anchor=tk.W)
        status_bar.pack(side=tk.BOTTOM, fill=tk.X)
    
    def setup_add_tab(self, parent):
        """设置添加学生界面"""
        frame = ttk.LabelFrame(parent, text="添加新学生", padding=10)
        frame.pack(fill='both', expand=True, padx=10, pady=10)
        
        # 学号
        ttk.Label(frame, text="学号:").grid(row=0, column=0, sticky='w', pady=5)
        self.add_id_var = tk.StringVar()
        ttk.Entry(frame, textvariable=self.add_id_var, width=30).grid(row=0, column=1, pady=5)
        
        # 姓名
        ttk.Label(frame, text="姓名:").grid(row=1, column=0, sticky='w', pady=5)
        self.add_name_var = tk.StringVar()
        ttk.Entry(frame, textvariable=self.add_name_var, width=30).grid(row=1, column=1, pady=5)
        
        # 班级
        ttk.Label(frame, text="班级:").grid(row=2, column=0, sticky='w', pady=5)
        self.add_class_var = tk.StringVar()
        ttk.Entry(frame, textvariable=self.add_class_var, width=30).grid(row=2, column=1, pady=5)
        
        # 照片
        ttk.Label(frame, text="照片:").grid(row=3, column=0, sticky='w', pady=5)
        self.photo_path_var = tk.StringVar()
        ttk.Entry(frame, textvariable=self.photo_path_var, width=25).grid(row=3, column=1, pady=5, sticky='w')
        ttk.Button(frame, text="选择照片", 
                  command=self.select_photo).grid(row=3, column=2, pady=5, padx=5)
        
        # 添加按钮
        ttk.Button(frame, text="添加学生", 
                  command=self.add_student).grid(row=4, column=1, pady=20)
    
    def setup_query_tab(self, parent):
        """设置查询学生界面"""
        frame = ttk.LabelFrame(parent, text="查询学生", padding=10)
        frame.pack(fill='both', expand=True, padx=10, pady=10)
        
        # 查询框
        ttk.Label(frame, text="输入学号:").grid(row=0, column=0, sticky='w', pady=5)
        self.query_id_var = tk.StringVar()
        ttk.Entry(frame, textvariable=self.query_id_var, width=30).grid(row=0, column=1, pady=5)
        ttk.Button(frame, text="查询", 
                  command=self.query_student).grid(row=0, column=2, pady=5, padx=5)
        
        # 结果显示区域
        result_frame = ttk.LabelFrame(frame, text="学生信息", padding=10)
        result_frame.grid(row=1, column=0, columnspan=3, sticky='nsew', pady=10)
        
        # 创建滚动文本区域
        self.result_text = tk.Text(result_frame, height=15, width=70)
        scrollbar = ttk.Scrollbar(result_frame, orient='vertical', 
                                 command=self.result_text.yview)
        self.result_text.configure(yscrollcommand=scrollbar.set)
        
        self.result_text.grid(row=0, column=0, sticky='nsew')
        scrollbar.grid(row=0, column=1, sticky='ns')
        
        # 配置网格权重
        result_frame.columnconfigure(0, weight=1)
        result_frame.rowconfigure(0, weight=1)
        
        # 配置frame权重
        frame.columnconfigure(1, weight=1)
        frame.rowconfigure(1, weight=1)
    
    def setup_list_tab(self, parent):
        """设置学生列表界面"""
        frame = ttk.Frame(parent)
        frame.pack(fill='both', expand=True, padx=10, pady=10)
        
        # 工具栏
        toolbar = ttk.Frame(frame)
        toolbar.pack(fill='x', pady=(0, 10))
        
        ttk.Button(toolbar, text="刷新列表", 
                  command=self.refresh_list).pack(side='left')
        
        # 创建树状视图
        columns = ('学号', '姓名', '班级')
        self.tree = ttk.Treeview(frame, columns=columns, show='headings', height=20)
        
        # 设置列标题
        for col in columns:
            self.tree.heading(col, text=col)
            self.tree.column(col, width=150)
        
        # 添加滚动条
        scrollbar = ttk.Scrollbar(frame, orient='vertical', command=self.tree.yview)
        self.tree.configure(yscrollcommand=scrollbar.set)
        
        self.tree.pack(side='left', fill='both', expand=True)
        scrollbar.pack(side='right', fill='y')
    
    def setup_delete_tab(self, parent):
        """设置删除学生界面"""
        frame = ttk.LabelFrame(parent, text="删除学生", padding=10)
        frame.pack(fill='both', expand=True, padx=10, pady=10)
        
        # 输入框
        ttk.Label(frame, text="输入学号:").grid(row=0, column=0, sticky='w', pady=5)
        self.delete_id_var = tk.StringVar()
        ttk.Entry(frame, textvariable=self.delete_id_var, width=30).grid(row=0, column=1, pady=5)
        
        # 按钮
        ttk.Button(frame, text="删除学生", 
                  command=self.delete_student).grid(row=1, column=1, pady=20)
    
    def select_photo(self):
        """选择照片文件"""
        filename = filedialog.askopenfilename(
            title="选择学生照片",
            filetypes=[("Image files", "*.jpg *.jpeg *.png"), ("All files", "*.*")]
        )
        if filename:
            self.photo_path_var.set(filename)
    
    def send_request(self, method, headers=None, body=None):
        """发送SHP请求到服务器"""
        try:
            # 创建socket连接
            client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client.settimeout(5)  # 5秒超时
            client.connect((self.server_host, self.server_port))
            
            # 构建请求
            request = f"{method} / SHP/1.0\r\n"
            
            if headers:
                for key, value in headers.items():
                    request += f"{key}: {value}\r\n"
            
            request += "\r\n"
            
            if body:
                request += body
            
            # 发送请求
            client.send(request.encode('utf-8'))
            
            # 接收响应
            response_data = b""
            while True:
                chunk = client.recv(4096)
                if not chunk:
                    break
                response_data += chunk
            
            client.close()
            
            # 解析响应
            response_str = response_data.decode('utf-8')
            header_end = response_str.find('\r\n\r\n')
            headers_str = response_str[:header_end]
            body_str = response_str[header_end + 4:]
            
            # 解析状态行
            status_line = headers_str.split('\r\n')[0]
            _, status_code, status_text = status_line.split(' ', 2)
            
            return int(status_code), status_text, body_str
            
        except Exception as e:
            messagebox.showerror("连接错误", f"无法连接到服务器: {str(e)}")
            return None, None, None
    
    def add_student(self):
        """添加学生"""
        student_id = self.add_id_var.get().strip()
        name = self.add_name_var.get().strip()
        class_name = self.add_class_var.get().strip()
        photo_path = self.photo_path_var.get().strip()
        
        if not student_id or not name:
            messagebox.showwarning("输入错误", "学号和姓名不能为空")
            return
        
        # 准备数据
        student_data = {
            'student_id': student_id,
            'name': name,
            'class': class_name,
            'last_updated': '2024-01-01'  # 实际应用中应该用当前时间
        }
        
        # 如果有照片，读取并编码为base64
        if photo_path and os.path.exists(photo_path):
            try:
                with open(photo_path, 'rb') as f:
                    photo_data = f.read()
                student_data['photo'] = base64.b64encode(photo_data).decode('utf-8')
            except Exception as e:
                messagebox.showwarning("照片错误", f"无法读取照片: {str(e)}")
        
        # 发送POST请求
        headers = {
            'Content-Type': 'application/json',
            'Content-Length': str(len(json.dumps(student_data)))
        }
        
        status_code, status_text, body = self.send_request(
            'POST', headers, json.dumps(student_data)
        )
        
        if status_code == 201:
            messagebox.showinfo("成功", "学生添加成功")
            self.clear_add_form()
            self.status_var.set(f"学生 {student_id} 添加成功")
        else:
            messagebox.showerror("错误", f"添加失败: {status_text}")
    
    def query_student(self):
        """查询学生"""
        student_id = self.query_id_var.get().strip()
        
        if not student_id:
            messagebox.showwarning("输入错误", "请输入学号")
            return
        
        headers = {'Student-ID': student_id}
        status_code, status_text, body = self.send_request('GET', headers)
        
        self.result_text.delete(1.0, tk.END)
        
        if status_code == 200:
            try:
                student_info = json.loads(body)
                formatted_info = json.dumps(student_info, indent=2, ensure_ascii=False)
                self.result_text.insert(1.0, formatted_info)
                self.status_var.set(f"查询到学生 {student_id}")
            except:
                self.result_text.insert(1.0, body)
        else:
            self.result_text.insert(1.0, f"查询失败: {status_text}")
            self.status_var.set(f"查询失败: {status_text}")
    
    def refresh_list(self):
        """刷新学生列表"""
        status_code, status_text, body = self.send_request('LIST')
        
        if status_code == 200:
            try:
                students = json.loads(body)
                
                # 清空现有数据
                for item in self.tree.get_children():
                    self.tree.delete(item)
                
                # 添加新数据
                for student in students:
                    self.tree.insert('', 'end', values=(
                        student.get('student_id', ''),
                        student.get('name', ''),
                        student.get('class', '')
                    ))
                
                self.status_var.set(f"已加载 {len(students)} 名学生")
                
            except Exception as e:
                messagebox.showerror("错误", f"解析数据失败: {str(e)}")
        else:
            messagebox.showerror("错误", f"获取列表失败: {status_text}")
    
    def delete_student(self):
        """删除学生"""
        student_id = self.delete_id_var.get().strip()
        
        if not student_id:
            messagebox.showwarning("输入错误", "请输入学号")
            return
        
        if not messagebox.askyesno("确认删除", f"确定要删除学号为 {student_id} 的学生吗？"):
            return
        
        headers = {'Student-ID': student_id}
        status_code, status_text, body = self.send_request('DELETE', headers)
        
        if status_code == 200:
            messagebox.showinfo("成功", "学生删除成功")
            self.delete_id_var.set("")
            self.status_var.set(f"学生 {student_id} 已删除")
            self.refresh_list()  # 刷新列表
        else:
            messagebox.showerror("错误", f"删除失败: {status_text}")
    
    def clear_add_form(self):
        """清空添加表单"""
        self.add_id_var.set("")
        self.add_name_var.set("")
        self.add_class_var.set("")
        self.photo_path_var.set("")

def main():
    root = tk.Tk()
    app = StudentClientGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()