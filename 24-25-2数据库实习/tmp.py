import sys
from PyQt5.QtWidgets import *

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("社团信息管理系统")
        self.setGeometry(100, 100, 600, 400)
        label=QLabel("欢迎使用社团信息管理系统", self)
        self.button = QPushButton("查看社团人员信息", self)
        self.button.setGeometry(20, 80, 200, 40)
        self.button.clicked.connect(self.bottom_select)
    def bottom_select():
        print("按钮被点击了")
if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())

