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