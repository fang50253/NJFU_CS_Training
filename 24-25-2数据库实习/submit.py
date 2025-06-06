import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import Qt

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("社团信息管理系统")
        self.setFixedSize(600, 400)
        
        # 主窗口中心部件
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # 使用网格布局
        layout = QGridLayout(central_widget)
        
        # 标题标签
        title_label = QLabel("欢迎使用社团信息管理系统")
        title_label.setAlignment(Qt.AlignCenter)
        title_label.setStyleSheet("""
            font-size: 24px; 
            font-weight: bold; 
            margin-bottom: 30px;
            color: #2c3e50;
        """)
        layout.addWidget(title_label, 0, 0, 1, 2)
        
        # 创建4个主功能按钮
        buttons = [
            ("学生信息", self.show_student_info),
            ("社团信息", self.show_club_info),
            ("指导老师信息", self.show_teacher_info),
            ("社团活动", self.show_activity_info)
        ]
        
        # 主按钮样式
        main_button_style = """
            QPushButton {
                background-color: #3498db;
                border: none;
                color: white;
                padding: 15px;
                text-align: center;
                font-size: 16px;
                margin: 10px;
                min-width: 200px;
                border-radius: 5px;
            }
            QPushButton:hover {
                background-color: #2980b9;
            }
        """
        
        # 添加主功能按钮
        for i, (text, handler) in enumerate(buttons):
            button = QPushButton(text)
            button.setStyleSheet(main_button_style)
            button.clicked.connect(handler)
            row = (i // 2) + 1
            col = i % 2
            layout.addWidget(button, row, col)
    
    def show_student_info(self):
        self.show_sub_window("学生信息管理")
    
    def show_club_info(self):
        self.show_sub_window("社团信息管理")
    
    def show_teacher_info(self):
        self.show_sub_window("指导老师信息管理")
    
    def show_activity_info(self):
        self.show_sub_window("社团活动管理")
    
    def show_sub_window(self, title):
        """显示带CRUD按钮的子窗口"""
        sub_window = QDialog(self)
        sub_window.setWindowTitle(title)
        sub_window.setFixedSize(500, 350)
        
        # 子窗口布局
        layout = QVBoxLayout(sub_window)
        
        # 标题
        label = QLabel(f"{title}系统")
        label.setAlignment(Qt.AlignCenter)
        label.setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 30px;")
        layout.addWidget(label)
        
        # 创建CRUD按钮
        crud_buttons = [
            ("新增", lambda: self.show_operation_page(title, "新增")),
            ("删除", lambda: self.show_operation_page(title, "删除")),
            ("修改", lambda: self.show_operation_page(title, "修改")),
            ("查询", lambda: self.show_operation_page(title, "查询"))
        ]
        
        # CRUD按钮样式 (蓝色)
        crud_button_style = """
            QPushButton {
                background-color: #3498db;
                border: none;
                color: white;
                padding: 12px;
                text-align: center;
                font-size: 14px;
                margin: 8px;
                min-width: 150px;
                border-radius: 4px;
            }
            QPushButton:hover {
                background-color: #2980b9;
            }
        """
        
        # 添加CRUD按钮
        for text, handler in crud_buttons:
            btn = QPushButton(text)
            btn.setStyleSheet(crud_button_style)
            btn.clicked.connect(handler)
            layout.addWidget(btn)
        
        # 添加一些间距
        layout.addStretch()
        
        sub_window.exec_()
    
    def show_operation_page(self, category, operation):
        """显示操作页面"""
        op_window = QDialog(self)
        op_window.setWindowTitle(f"{category}-{operation}")
        op_window.setFixedSize(500, 350)
        
        layout = QVBoxLayout(op_window)
        
        # 操作提示
        label = QLabel(f"正在执行: {category} {operation}操作")
        label.setAlignment(Qt.AlignCenter)
        label.setStyleSheet("font-size: 18px; margin-bottom: 30px;")
        layout.addWidget(label)
        
        # 这里可以添加具体的操作控件
        if operation == "新增":
            # 示例: 添加表单控件
            form_layout = QFormLayout()
            form_layout.addRow("名称:", QLineEdit())
            form_layout.addRow("描述:", QTextEdit())
            layout.addLayout(form_layout)
            
            submit_btn = QPushButton("提交")
            submit_btn.setStyleSheet("background-color: #2ecc71; color: white; padding: 10px;")
            layout.addWidget(submit_btn)
        
        elif operation == "查询":
            # 示例: 添加查询结果表格
            table = QTableWidget()
            table.setColumnCount(3)
            table.setHorizontalHeaderLabels(["ID", "名称", "状态"])
            table.setRowCount(5)  # 示例数据
            layout.addWidget(table)
        
        # 其他操作类似可以继续添加
        
        layout.addStretch()
        op_window.exec_()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())