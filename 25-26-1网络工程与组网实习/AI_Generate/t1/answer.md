这是一个非常棒的实践项目，我将为你设计一个基于TCP的类HTTP协议，并给出完整的实现架构。

## 一、协议设计：SHP（Simple HTTP-like Protocol）

### 1. 协议基础
```
通信方式：TCP
默认端口：8888
编码格式：UTF-8
传输格式：纯文本头部 + 可选二进制内容
```

### 2. 报文格式

**请求报文：**
```
<方法> <路径> SHP/1.0\r\n
<头部字段1>: <值>\r\n
<头部字段2>: <值>\r\n
\r\n
<可选的消息体>
```

**响应报文：**
```
SHP/1.0 <状态码> <状态描述>\r\n
<头部字段1>: <值>\r\n
<头部字段2>: <值>\r\n
\r\n
<可选的消息体>
```

### 3. 方法定义
| 方法 | 功能 | 必需头部 | 消息体 |
|------|------|----------|--------|
| GET | 查询学生 | Student-ID（可选） | 空 |
| POST | 添加学生 | Content-Type, Content-Length | JSON数据 |
| PUT | 更新学生 | Student-ID, Content-Type, Content-Length | JSON数据 |
| DELETE | 删除学生 | Student-ID | 空 |
| LIST | 列出所有学生 | 无 | 空 |

### 4. 状态码
```
200 OK - 请求成功
201 Created - 创建成功
400 Bad Request - 请求格式错误
404 Not Found - 学生不存在
500 Internal Server Error - 服务器错误
```

### 5. 头部字段
```
Student-ID: 学号（字符串）
Content-Type: 数据类型（application/json, image/jpeg等）
Content-Length: 消息体长度
Photo-Size: 照片大小（字节）
Photo-Format: 照片格式（jpg, png等）
```

## 二、服务器端实现（Python）

```python
import json
import socket
import threading
import os
import base64
from datetime import datetime

class StudentServer:
    def __init__(self, host='0.0.0.0', port=8888):
        self.host = host
        self.port = port
        self.students = {}  # 学号 -> 学生信息
        self.photos_dir = 'student_photos'
        os.makedirs(self.photos_dir, exist_ok=True)
        
    def start(self):
        """启动服务器"""
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((self.host, self.port))
        server.listen(5)
        print(f"服务器启动在 {self.host}:{self.port}")
        
        while True:
            client, address = server.accept()
            thread = threading.Thread(target=self.handle_client, args=(client, address))
            thread.start()
    
    def handle_client(self, client, address):
        """处理客户端请求"""
        try:
            # 接收请求
            request_data = client.recv(4096).decode('utf-8')
            if not request_data:
                return
            
            # 解析请求
            response = self.parse_request(request_data, client)
            
            # 发送响应
            client.send(response.encode('utf-8'))
            
        except Exception as e:
            error_response = self.create_response(500, f"Server Error: {str(e)}")
            client.send(error_response.encode('utf-8'))
        finally:
            client.close()
    
    def parse_request(self, request_data, client):
        """解析请求并返回响应"""
        lines = request_data.split('\r\n')
        if len(lines) < 1:
            return self.create_response(400, "Bad Request")
        
        # 解析请求行
        request_line = lines[0]
        parts = request_line.split()
        if len(parts) < 3:
            return self.create_response(400, "Bad Request")
        
        method, path, version = parts[0], parts[1], parts[2]
        
        # 解析头部
        headers = {}
        body = None
        body_started = False
        body_lines = []
        
        for line in lines[1:]:
            if not line and not body_started:
                body_started = True
                continue
            
            if not body_started:
                if ': ' in line:
                    key, value = line.split(': ', 1)
                    headers[key] = value
            else:
                body_lines.append(line)
        
        if body_lines:
            body = '\r\n'.join(body_lines)
        
        # 根据方法处理请求
        if method == 'GET':
            return self.handle_get(headers)
        elif method == 'POST':
            return self.handle_post(headers, body)
        elif method == 'PUT':
            return self.handle_put(headers, body)
        elif method == 'DELETE':
            return self.handle_delete(headers)
        elif method == 'LIST':
            return self.handle_list()
        else:
            return self.create_response(400, "Method Not Allowed")
    
    def handle_get(self, headers):
        """处理GET请求 - 查询学生"""
        student_id = headers.get('Student-ID')
        
        if not student_id:
            # 如果没有提供学号，返回所有学生列表（简化版）
            return self.create_response(200, "OK", 
                                       content_type="application/json",
                                       body=json.dumps(list(self.students.keys())))
        
        if student_id not in self.students:
            return self.create_response(404, "Student Not Found")
        
        student = self.students[student_id]
        return self.create_response(200, "OK", 
                                   content_type="application/json",
                                   body=json.dumps(student))
    
    def handle_post(self, headers, body):
        """处理POST请求 - 添加学生"""
        try:
            if not body:
                return self.create_response(400, "No Data Provided")
            
            data = json.loads(body)
            student_id = data.get('student_id')
            
            if not student_id:
                return self.create_response(400, "Student ID Required")
            
            if student_id in self.students:
                return self.create_response(400, "Student Already Exists")
            
            # 处理照片（如果有）
            if 'photo' in data and data['photo']:
                photo_data = base64.b64decode(data['photo'])
                photo_path = os.path.join(self.photos_dir, f"{student_id}.jpg")
                with open(photo_path, 'wb') as f:
                    f.write(photo_data)
                data['photo_path'] = photo_path
                del data['photo']  # 移除base64数据，节省内存
            
            self.students[student_id] = data
            return self.create_response(201, "Created")
            
        except json.JSONDecodeError:
            return self.create_response(400, "Invalid JSON")
    
    def handle_put(self, headers, body):
        """处理PUT请求 - 更新学生"""
        try:
            student_id = headers.get('Student-ID')
            if not student_id:
                return self.create_response(400, "Student-ID Header Required")
            
            if student_id not in self.students:
                return self.create_response(404, "Student Not Found")
            
            if not body:
                return self.create_response(400, "No Data Provided")
            
            data = json.loads(body)
            # 更新学生信息
            self.students[student_id].update(data)
            
            return self.create_response(200, "Updated")
            
        except json.JSONDecodeError:
            return self.create_response(400, "Invalid JSON")
    
    def handle_delete(self, headers):
        """处理DELETE请求 - 删除学生"""
        student_id = headers.get('Student-ID')
        if not student_id:
            return self.create_response(400, "Student-ID Header Required")
        
        if student_id not in self.students:
            return self.create_response(404, "Student Not Found")
        
        # 删除照片文件
        photo_path = os.path.join(self.photos_dir, f"{student_id}.jpg")
        if os.path.exists(photo_path):
            os.remove(photo_path)
        
        del self.students[student_id]
        return self.create_response(200, "Deleted")
    
    def handle_list(self):
        """处理LIST请求 - 列出所有学生"""
        students_list = []
        for student_id, info in self.students.items():
            student_info = {
                'student_id': student_id,
                'name': info.get('name', ''),
                'class': info.get('class', ''),
                'last_updated': info.get('last_updated', datetime.now().isoformat())
            }
            students_list.append(student_info)
        
        return self.create_response(200, "OK",
                                   content_type="application/json",
                                   body=json.dumps(students_list))
    
    def create_response(self, status_code, status_text, 
                       content_type="text/plain", body=""):
        """创建响应报文"""
        response = f"SHP/1.0 {status_code} {status_text}\r\n"
        response += f"Content-Type: {content_type}\r\n"
        response += f"Content-Length: {len(body.encode('utf-8'))}\r\n"
        response += f"Date: {datetime.now().strftime('%a, %d %b %Y %H:%M:%S GMT')}\r\n"
        response += "\r\n"
        response += body
        return response

if __name__ == "__main__":
    server = StudentServer()
    server.start()
```

## 三、客户端图形界面（Tkinter实现）

```python
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
```

## 四、使用说明

### 1. 启动服务器
```bash
python student_server.py
```

### 2. 启动客户端
```bash
python student_client.py
```

### 3. 测试命令示例

**通过命令行测试（可选）：**
```bash
# 添加学生
echo '{"student_id": "1001", "name": "张三", "class": "计算机1班"}' | \
nc localhost 8888
```

## 五、协议特点

1. **简单性**：类似HTTP的文本协议，易于调试
2. **扩展性**：支持各种学生信息操作
3. **二进制支持**：通过Base64编码传输照片
4. **RESTful风格**：使用标准HTTP方法语义
5. **图形界面**：提供直观的客户端操作界面

这个设计包含了完整的协议规范、服务器实现和图形客户端，可以直接运行和扩展。