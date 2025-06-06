import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
import pymysql

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("社团信息管理系统")
        self.setFixedSize(790, 600)
        bottom_add_student= QPushButton("添加学生信息", self)
        bottom_add_student.setGeometry(20, 20, 200, 40)
        bottom_add_student.clicked.connect(self.add_student_info)
        bottom_add_teacher= QPushButton("添老师信息", self)
        bottom_add_teacher.setGeometry(20, 70, 200, 40)
        bottom_add_teacher.clicked.connect(self.add_teacher_info)
        bottom_add_club= QPushButton("添社团信息", self)
        bottom_add_club.setGeometry(20, 120, 200, 40)
        bottom_add_club.clicked.connect(self.add_club_info)
        bottom_add_event= QPushButton("添加活动信息", self)
        bottom_add_event.setGeometry(20, 170, 200, 40)
        bottom_add_event.clicked.connect(self.add_event_info)
        bottom_add_student= QPushButton("查看学生信息", self)
        bottom_add_student.setGeometry(270, 20, 200, 40)
        bottom_add_student.clicked.connect(self.show_student_info)
        bottom_add_teacher= QPushButton("查看老师信息", self)
        bottom_add_teacher.setGeometry(270, 70, 200, 40)
        bottom_add_teacher.clicked.connect(self.show_teacher_info)
        bottom_add_club= QPushButton("查看社团信息", self)
        bottom_add_club.setGeometry(270, 120, 200, 40)
        bottom_add_club.clicked.connect(self.show_club_info)
        bottom_add_event= QPushButton("查看活动信息", self)
        bottom_add_event.setGeometry(270, 170, 200, 40)
        bottom_add_event.clicked.connect(self.show_event_info)
        bottom_edit_student= QPushButton("修改学生信息", self)
        bottom_edit_student.setGeometry(520, 20, 200, 40)
        bottom_edit_student.clicked.connect(self.edit_student_info)
        bottom_edit_teacher= QPushButton("修改老师信息", self)
        bottom_edit_teacher.setGeometry(520, 70, 200, 40)
        bottom_edit_teacher.clicked.connect(self.edit_teacher_info)
        bottom_edit_club= QPushButton("修改社团信息", self)
        bottom_edit_club.setGeometry(520, 120, 200, 40)
        bottom_edit_club.clicked.connect(self.edit_club_info)
        bottom_edit_event= QPushButton("修改活动信息", self)
        bottom_edit_event.setGeometry(520, 170, 200, 40)
        bottom_edit_event.clicked.connect(self.edit_event_info)
    def show_student_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('学生信息')
        sub_window.setFixedSize(800, 400)
        layout = QVBoxLayout()
        table = QTableWidget()
        table.setColumnCount(5)
        table.setHorizontalHeaderLabels(['学号', '姓名', '性别', '班级', '专业'])
        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                charset='utf8mb4'
            )
            with connection.cursor() as cursor:
                cursor.execute("SELECT * from students")
                students = cursor.fetchall()
                table.setRowCount(len(students))
                for row, student in enumerate(students):
                    cursor.execute("SELECT * FROM students WHERE sid = %s", (student[0],))
                    data = cursor.fetchone()
                    for col, value in enumerate(data):
                        table.setItem(row, col, QTableWidgetItem(str(value)))
                for row, student in enumerate(students):
                    for col, value in enumerate(student):
                        # 处理性别显示
                        if col == 2: 
                            value = '男' if value == 'm' else '女' if value == 'f' else value
                        table.setItem(row, col, QTableWidgetItem(str(value)))
            connection.commit()
        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            if 'connection' in locals():
                connection.rollback()
        finally:
            if 'connection' in locals() and connection.open:
                connection.close()
        layout.addWidget(table)
        sub_window.setLayout(layout)
        sub_window.exec_()
    def show_teacher_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('老师信息')
        sub_window.setFixedSize(800, 400)
        layout = QVBoxLayout()
        table = QTableWidget()
        table.setColumnCount(3)
        table.setHorizontalHeaderLabels(['教工号', '姓名', '性别'])
        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                charset='utf8mb4'
            )
            with connection.cursor() as cursor:
                cursor.execute("SELECT * from teachers")
                teachers = cursor.fetchall()
                table.setRowCount(len(teachers))
                for row, teacher in enumerate(teachers):
                    cursor.execute("SELECT * FROM teachers WHERE tid = %s", (teacher[0],))
                    data = cursor.fetchone()
                    for col, value in enumerate(data):
                        table.setItem(row, col, QTableWidgetItem(str(value)))
                for row, teacher in enumerate(teachers):
                    for col, value in enumerate(teacher):
                        # 处理性别显示
                        if col == 2:  # 老师性别列
                            value = '男' if value == 'm' else '女' if value == 'f' else value
                        table.setItem(row, col, QTableWidgetItem(str(value)))
            connection.commit()
        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            if 'connection' in locals():
                connection.rollback()
        finally:
            if 'connection' in locals() and connection.open:
                connection.close()
        layout.addWidget(table)
        sub_window.setLayout(layout)
        sub_window.exec_()
    def show_club_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('社团信息')
        sub_window.setFixedSize(1000, 600)  # 增大窗口尺寸以容纳更多信息
        layout = QVBoxLayout()
        
        # 创建表格
        table = QTableWidget()
        table.setColumnCount(6)  # 增加列数
        table.setHorizontalHeaderLabels(['社团编号', '社团名称', '成员人数', '指导老师工号', '指导老师姓名', '指导老师性别'])
        
        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                charset='utf8mb4'
            )
            
            with connection.cursor() as cursor:
                # 获取所有社团信息
                cursor.execute("""
                    SELECT 
                        clubs.cid, 
                        clubs.name, 
                        COUNT(DISTINCT sc.sid) AS member_count,
                        teachers.tid, 
                        teachers.name AS teacher_name, 
                        teachers.sex AS teacher_sex
                    FROM clubs, sc, tc, teachers
                    WHERE clubs.cid = sc.cid 
                    AND clubs.cid = tc.cid 
                    AND tc.tid = teachers.tid
                    GROUP BY clubs.cid, clubs.name, teachers.tid, teachers.name, teachers.sex;
                """)
                
                clubs = cursor.fetchall()
                table.setRowCount(len(clubs))
                
                for row, club in enumerate(clubs):
                    for col, value in enumerate(club):
                        # 处理性别显示
                        if col == 5:  # 老师性别列
                            value = '男' if value == 'm' else '女' if value == 'f' else value
                        table.setItem(row, col, QTableWidgetItem(str(value)))
            
            connection.commit()
        
        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            if 'connection' in locals():
                connection.rollback()
        
        finally:
            if 'connection' in locals() and connection.open:
                connection.close()
        
        # 设置表格属性
        table.setEditTriggers(QTableWidget.NoEditTriggers)  # 禁止编辑
        table.setSelectionBehavior(QTableWidget.SelectRows)  # 整行选择
        table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)  # 自动调整列宽
        
        layout.addWidget(table)
        sub_window.setLayout(layout)
        sub_window.exec_()

    def show_event_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('活动信息')
        sub_window.setFixedSize(800, 400)
        layout = QVBoxLayout()
        table = QTableWidget()
        table.setColumnCount(5)
        table.setHorizontalHeaderLabels(['活动ID', '活动名称', '活动地点', '社团ID', '活动日期'])
        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
            )
            with connection.cursor() as cursor:
                cursor.execute("SELECT eid from events")
                events = cursor.fetchall()
                table.setRowCount(len(events))
                for row, events in enumerate(events):
                    cursor.execute("SELECT * FROM events WHERE eid = %s", (event[0],))
                    data = cursor.fetchone()
                    for col, value in enumerate(data):
                        table.setItem(row, col, QTableWidgetItem(str(value)))
            connection.commit()
        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            if 'connection' in locals():
                connection.rollback()
        finally:
            if 'connection' in locals() and connection.open:
                connection.close()
        layout.addWidget(table)
        sub_window.setLayout(layout)
        sub_window.exec_()
    def add_club_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('添加社团信息')
        sub_window.setFixedSize(600, 400)
        form_layout = QFormLayout()
        self.club_id_edit = QLineEdit()
        form_layout.addRow("社团编号*:", self.club_id_edit)
        self.club_name_edit = QLineEdit()
        form_layout.addRow("社团名称*:", self.club_name_edit)
        self.teacher_combo = QComboBox()
        self.load_teachers()
        form_layout.addRow("指导老师*:", self.teacher_combo)
        button_box = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        button_box.accepted.connect(lambda: self.save_club_data(sub_window))
        button_box.rejected.connect(sub_window.reject)
        form_layout.addRow(button_box)
        sub_window.setLayout(form_layout)
        sub_window.exec_()
    def add_student_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('添加学生信息')
        sub_window.setFixedSize(600, 400)
        form_layout = QFormLayout()
        self.student_id_edit = QLineEdit()
        self.student_name_edit = QLineEdit()
        self.gender_combo = QComboBox()
        self.gender_combo.addItems(['男', '女'])
        self.class_edit = QLineEdit()
        self.major_edit = QLineEdit()
        form_layout.addRow("学生学号*:", self.student_id_edit)
        form_layout.addRow("学生姓名*:", self.student_name_edit)
        form_layout.addRow("学生性别*:", self.gender_combo)
        form_layout.addRow("学生班级*:", self.class_edit)
        form_layout.addRow("学生专业*:", self.major_edit)
        button_box = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        button_box.accepted.connect(lambda: self.save_student_data(sub_window))
        button_box.rejected.connect(sub_window.reject)
        form_layout.addRow(button_box)
        sub_window.setLayout(form_layout)
        sub_window.exec_()
    def add_teacher_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('添加老师信息')
        sub_window.setFixedSize(600, 400)
        form_layout = QFormLayout()
        self.teacher_id_edit = QLineEdit()
        self.teacher_name_edit = QLineEdit()
        self.gender_combo = QComboBox()
        self.gender_combo.addItems(['男', '女'])
        self.class_edit = QLineEdit()
        self.major_edit = QLineEdit()
        form_layout.addRow("老师教工号*:", self.teacher_id_edit)
        form_layout.addRow("老师姓名*:", self.teacher_name_edit)
        form_layout.addRow("老师性别*:", self.gender_combo)
        button_box = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        button_box.accepted.connect(lambda: self.save_teacher_data(sub_window))
        button_box.rejected.connect(sub_window.reject)
        form_layout.addRow(button_box)
        sub_window.setLayout(form_layout)
        sub_window.exec_()
    def add_event_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('添加活动信息')
        sub_window.setFixedSize(600, 400)
        form_layout = QFormLayout()
        self.event_id_edit = QLineEdit()
        form_layout.addRow("活动ID*:", self.event_id_edit)
        self.event_name_edit = QLineEdit()
        form_layout.addRow("活动名称*:", self.event_name_edit)
        self.event_place_edit = QLineEdit()
        form_layout.addRow("活动地点*:", self.event_place_edit)
        self.club_id_edit = QLineEdit()
        form_layout.addRow("社团ID*:", self.club_id_edit)
        self.date_edit = QDateEdit()
        self.date_edit.setCalendarPopup(True)  # 启用日历弹出
        self.date_edit.setDate(QDate.currentDate())  # 默认当前日期
        self.date_edit.setDisplayFormat("yyyy-MM-dd")  # 设置日期格式
        form_layout.addRow("活动日期*:", self.date_edit)
        button_box = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        button_box.accepted.connect(lambda: self.save_event_data(sub_window))
        button_box.rejected.connect(sub_window.reject)
        form_layout.addRow(button_box)
        sub_window.setLayout(form_layout)
        sub_window.exec_()
    def save_event_data(self, dialog):
        # 获取输入数据
        eid = self.event_id_edit.text()
        name = self.event_name_edit.text()
        place = self.event_place_edit.text()
        cid = self.club_id_edit.text()
        date = self.date_edit.date().toString("yyyy-MM-dd")  # 获取格式化日期字符串
        # 验证数据
        if not all([eid, name, place, cid]):
            QMessageBox.warning(self, "警告", "请填写所有必填字段！")
            return
        try:
            # 连接数据库
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                charset='utf8mb4'
            )
            with connection.cursor() as cursor:
                # 插入活动数据
                sql = """
                INSERT INTO events (eid, name, place, cid, date)
                VALUES (%s, %s, %s, %s, %s)
                """
                cursor.execute(sql, (eid, name, place, cid, date))
            connection.commit()
            QMessageBox.information(self, "成功", "活动信息添加成功！")
            dialog.accept()
        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            if 'connection' in locals():
                connection.rollback()
        finally:
            if 'connection' in locals() and connection.open:
                connection.close()
    def save_student_data(self, dialog):
        student_id = self.student_id_edit.text()
        name = self.student_name_edit.text()
        gender = 'm' if self.gender_combo.currentText() == '男' else 'f'
        class_num = self.class_edit.text()
        major = self.major_edit.text()
        if not all([student_id, name, major]):
            QMessageBox.warning(self, "警告", "请填写所有必填字段！")
            return
        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                # charset='utf8mb4'
            )
            with connection.cursor() as cursor:
                # 插入学生数据
                sql = """
                INSERT INTO students (sid, name, sex, class, major)
                VALUES (%s, %s, %s, %s, %s)
                """
                cursor.execute(sql, (student_id, name, gender, class_num, major))
            connection.commit()
            QMessageBox.information(self, "成功", "学生信息添加成功！")
            dialog.accept()
        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            connection.rollback()
        finally:
            if 'connection' in locals() and connection.open:
                connection.close()
    def save_teacher_data(self, dialog):
        teacher_id = self.teacher_id_edit.text()
        name = self.teacher_name_edit.text()
        gender = 'm' if self.gender_combo.currentText() == '男' else 'f'
        if not all([teacher_id, name]):
            QMessageBox.warning(self, "警告", "请填写所有必填字段！")
            return
        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                # charset='utf8mb4'
            )
            with connection.cursor() as cursor:
                # 插入学生数据
                sql = """
                INSERT INTO teachers (tid, name, sex)
                VALUES (%s, %s, %s)
                """
                cursor.execute(sql, (teacher_id, name, gender))
            connection.commit()
            QMessageBox.information(self, "成功", "老师信息添加成功！")
            dialog.accept()
        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            connection.rollback()
        finally:
            if 'connection' in locals() and connection.open:
                connection.close()
    def save_club_data(self, dialog):
        club_id = self.club_id_edit.text()
        name = self.club_name_edit.text()
        teacher_data = self.teacher_combo.currentData()  # 获取选择的老师ID
        
        if not all([club_id, name, teacher_data]):
            QMessageBox.warning(self, "警告", "请填写所有必填字段！")
            return
        
        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                charset='utf8mb4'
            )
            
            with connection.cursor() as cursor:
                # 插入社团数据
                cursor.execute("INSERT INTO clubs (cid, name) VALUES (%s, %s)", (club_id, name))
                
                # 插入社团-老师关联数据
                cursor.execute("INSERT INTO tc (tid, cid) VALUES (%s, %s)", (teacher_data, club_id))
            
            connection.commit()
            QMessageBox.information(self, "成功", "社团信息添加成功！")
            dialog.accept()
        
        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            if 'connection' in locals():
                connection.rollback()
        
        finally:
            if 'connection' in locals() and connection.open:
                connection.close()
    def load_teachers(self):
        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                charset='utf8mb4'
            )
            
            with connection.cursor() as cursor:
                cursor.execute("SELECT tid, name FROM teachers")
                teachers = cursor.fetchall()
                self.teacher_combo.clear()
                for tid, name in teachers:
                    self.teacher_combo.addItem(f"{name} ({tid})", tid)
        
        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"加载老师列表失败: {str(e)}")
        
        finally:
            if 'connection' in locals() and connection.open:
                connection.close()
    def edit_student_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('编辑学生信息')
        sub_window.setFixedSize(900, 600)
        layout = QVBoxLayout()

        table = QTableWidget()
        table.setColumnCount(5)
        table.setHorizontalHeaderLabels(['学号', '姓名', '性别', '班级', '专业'])

        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                charset='utf8mb4'
            )

            with connection.cursor() as cursor:
                cursor.execute("SELECT sid, name, sex, class, major FROM students")
                students = cursor.fetchall()
                table.setRowCount(len(students))

                for row, student in enumerate(students):
                    sid, name, sex, clazz, major = student
                    table.setItem(row, 0, QTableWidgetItem(str(sid)))
                    table.item(row, 0).setFlags(table.item(row, 0).flags() & ~Qt.ItemIsEditable)  # sid 不可编辑

                    table.setItem(row, 1, QTableWidgetItem(name))

                    sex_text = '男' if sex == 'm' else '女'
                    table.setItem(row, 2, QTableWidgetItem(sex_text))

                    table.setItem(row, 3, QTableWidgetItem(str(clazz)))
                    table.setItem(row, 4, QTableWidgetItem(major))

            connection.commit()

        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            if 'connection' in locals():
                connection.rollback()

        finally:
            if 'connection' in locals() and connection.open:
                connection.close()

        table.setEditTriggers(QTableWidget.DoubleClicked)
        table.setSelectionBehavior(QTableWidget.SelectRows)
        table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

        btn_layout = QHBoxLayout()

        # 保存按钮
        save_btn = QPushButton("保存修改")
        def save_edit_student_changes():
            try:
                connection = pymysql.connect(
                    host='mysql5.sqlpub.com',
                    port=3310,
                    user='fzy50253',
                    password='r8dooWY68Hbot6C7',
                    database='fzy2025',
                    charset='utf8mb4'
                )
                with connection.cursor() as cursor:
                    for row in range(table.rowCount()):
                        sid = table.item(row, 0).text()
                        name = table.item(row, 1).text()
                        sex_text = table.item(row, 2).text()
                        sex = 'm' if sex_text == '男' else 'f' if sex_text == '女' else None
                        clazz = table.item(row, 3).text()
                        major = table.item(row, 4).text()

                        if not all([sid, name, sex, clazz, major]):
                            continue

                        cursor.execute("""
                            UPDATE students
                            SET name=%s, sex=%s, class=%s, major=%s
                            WHERE sid=%s
                        """, (name, sex, clazz, major, sid))

                connection.commit()
                QMessageBox.information(self, "成功", "学生信息已保存。")
            except pymysql.Error as e:
                QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
                if 'connection' in locals():
                    connection.rollback()
            finally:
                if 'connection' in locals() and connection.open:
                    connection.close()

        save_btn.clicked.connect(save_edit_student_changes)
        btn_layout.addWidget(save_btn)

        # 删除按钮
        del_btn = QPushButton("删除选中行")
        def delete_row():
            selected = table.selectedItems()
            if not selected:
                QMessageBox.warning(sub_window, "提示", "请选择要删除的学生行")
                return
            row = selected[0].row()
            sid = table.item(row, 0).text()
            ret = QMessageBox.question(sub_window, "确认删除", f"确认删除学号为 {sid} 的学生吗？", QMessageBox.Yes | QMessageBox.No)
            if ret == QMessageBox.Yes:
                try:
                    connection = pymysql.connect(
                        host='mysql5.sqlpub.com',
                        port=3310,
                        user='fzy50253',
                        password='r8dooWY68Hbot6C7',
                        database='fzy2025',
                        charset='utf8mb4'
                    )
                    with connection.cursor() as cursor:
                        cursor.execute("DELETE FROM sc WHERE sid=%s", (sid,))
                        cursor.execute("DELETE FROM se WHERE sid=%s", (sid,))
                        cursor.execute("DELETE FROM students WHERE sid=%s", (sid,))

                    connection.commit()
                    table.removeRow(row)
                    QMessageBox.information(sub_window, "成功", f"学号为 {sid} 的学生已删除。")
                except pymysql.Error as e:
                    QMessageBox.critical(sub_window, "错误", f"数据库错误: {str(e)}")
                    if 'connection' in locals():
                        connection.rollback()
                finally:
                    if 'connection' in locals() and connection.open:
                        connection.close()

        del_btn.clicked.connect(delete_row)
        btn_layout.addWidget(del_btn)

        layout.addWidget(table)
        layout.addLayout(btn_layout)
        sub_window.setLayout(layout)
        sub_window.exec_()
    def edit_teacher_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('编辑老师信息')
        sub_window.setFixedSize(600, 400)
        layout = QVBoxLayout()

        table = QTableWidget()
        table.setColumnCount(3)
        table.setHorizontalHeaderLabels(['教工号', '姓名', '性别'])

        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                charset='utf8mb4'
            )

            with connection.cursor() as cursor:
                cursor.execute("SELECT tid, name, sex FROM teachers")
                teachers = cursor.fetchall()
                table.setRowCount(len(teachers))

                for row, teacher in enumerate(teachers):
                    tid, name, sex = teacher
                    table.setItem(row, 0, QTableWidgetItem(str(tid)))
                    table.item(row, 0).setFlags(table.item(row, 0).flags() & ~Qt.ItemIsEditable)

                    table.setItem(row, 1, QTableWidgetItem(name))
                    sex_text = '男' if sex == 'm' else '女'
                    table.setItem(row, 2, QTableWidgetItem(sex_text))

            connection.commit()

        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            if 'connection' in locals():
                connection.rollback()

        finally:
            if 'connection' in locals() and connection.open:
                connection.close()

        table.setEditTriggers(QTableWidget.DoubleClicked)
        table.setSelectionBehavior(QTableWidget.SelectRows)
        table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

        btn_layout = QHBoxLayout()

        save_btn = QPushButton("保存修改")
        def save_changes():
            try:
                connection = pymysql.connect(
                    host='mysql5.sqlpub.com',
                    port=3310,
                    user='fzy50253',
                    password='r8dooWY68Hbot6C7',
                    database='fzy2025',
                    charset='utf8mb4'
                )
                with connection.cursor() as cursor:
                    for row in range(table.rowCount()):
                        tid = table.item(row, 0).text()
                        name = table.item(row, 1).text()
                        sex_text = table.item(row, 2).text()
                        sex = 'm' if sex_text == '男' else 'f' if sex_text == '女' else None
                        if not all([tid, name, sex]):
                            continue

                        cursor.execute("""
                            UPDATE teachers SET name=%s, sex=%s WHERE tid=%s
                        """, (name, sex, tid))
                connection.commit()
                QMessageBox.information(self, "成功", "老师信息已保存。")
            except pymysql.Error as e:
                QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
                if 'connection' in locals():
                    connection.rollback()
            finally:
                if 'connection' in locals() and connection.open:
                    connection.close()

        save_btn.clicked.connect(save_changes)
        btn_layout.addWidget(save_btn)

        del_btn = QPushButton("删除选中行")
        def delete_row():
            selected = table.selectedItems()
            if not selected:
                QMessageBox.warning(sub_window, "提示", "请选择要删除的老师行")
                return
            row = selected[0].row()
            tid = table.item(row, 0).text()
            ret = QMessageBox.question(sub_window, "确认删除", f"确认删除工号为 {tid} 的老师吗？", QMessageBox.Yes | QMessageBox.No)
            if ret == QMessageBox.Yes:
                try:
                    connection = pymysql.connect(
                        host='mysql5.sqlpub.com',
                        port=3310,
                        user='fzy50253',
                        password='r8dooWY68Hbot6C7',
                        database='fzy2025',
                        charset='utf8mb4'
                    )
                    with connection.cursor() as cursor:
                        cursor.execute("DELETE FROM tc WHERE tid=%s", (tid,))
                        cursor.execute("DELETE FROM teachers WHERE tid=%s", (tid,))
                    connection.commit()
                    table.removeRow(row)
                    QMessageBox.information(sub_window, "成功", f"工号为 {tid} 的老师已删除。")
                except pymysql.Error as e:
                    QMessageBox.critical(sub_window, "错误", f"数据库错误: {str(e)}")
                    if 'connection' in locals():
                        connection.rollback()
                finally:
                    if 'connection' in locals() and connection.open:
                        connection.close()

        del_btn.clicked.connect(delete_row)
        btn_layout.addWidget(del_btn)

        layout.addWidget(table)
        layout.addLayout(btn_layout)
        sub_window.setLayout(layout)
        sub_window.exec_()
    def edit_club_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('编辑社团信息')
        sub_window.setFixedSize(600, 400)
        layout = QVBoxLayout()

        table = QTableWidget()
        table.setColumnCount(2)
        table.setHorizontalHeaderLabels(['社团编号', '社团名称'])

        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                charset='utf8mb4'
            )
            with connection.cursor() as cursor:
                cursor.execute("SELECT cid, name FROM clubs")
                clubs = cursor.fetchall()
                table.setRowCount(len(clubs))

                for row, club in enumerate(clubs):
                    cid, name = club
                    table.setItem(row, 0, QTableWidgetItem(str(cid)))
                    table.item(row, 0).setFlags(table.item(row, 0).flags() & ~Qt.ItemIsEditable)

                    table.setItem(row, 1, QTableWidgetItem(name))

            connection.commit()

        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            if 'connection' in locals():
                connection.rollback()
        finally:
            if 'connection' in locals() and connection.open:
                connection.close()

        table.setEditTriggers(QTableWidget.DoubleClicked)
        table.setSelectionBehavior(QTableWidget.SelectRows)
        table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

        btn_layout = QHBoxLayout()

        save_btn = QPushButton("保存修改")
        def save_changes():
            try:
                connection = pymysql.connect(
                    host='mysql5.sqlpub.com',
                    port=3310,
                    user='fzy50253',
                    password='r8dooWY68Hbot6C7',
                    database='fzy2025',
                    charset='utf8mb4'
                )
                with connection.cursor() as cursor:
                    for row in range(table.rowCount()):
                        cid = table.item(row, 0).text()
                        name = table.item(row, 1).text()
                        if not all([cid, name]):
                            continue
                        cursor.execute("UPDATE clubs SET name=%s WHERE cid=%s", (name, cid))
                connection.commit()
                QMessageBox.information(self, "成功", "社团信息已保存。")
            except pymysql.Error as e:
                QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
                if 'connection' in locals():
                    connection.rollback()
            finally:
                if 'connection' in locals() and connection.open:
                    connection.close()

        save_btn.clicked.connect(save_changes)
        btn_layout.addWidget(save_btn)

        del_btn = QPushButton("删除选中行")
        def delete_row():
            selected = table.selectedItems()
            if not selected:
                QMessageBox.warning(sub_window, "提示", "请选择要删除的社团行")
                return
            row = selected[0].row()
            cid = table.item(row, 0).text()
            ret = QMessageBox.question(sub_window, "确认删除", f"确认删除社团编号为 {cid} 的社团吗？", QMessageBox.Yes | QMessageBox.No)
            if ret == QMessageBox.Yes:
                try:
                    connection = pymysql.connect(
                        host='mysql5.sqlpub.com',
                        port=3310,
                        user='fzy50253',
                        password='r8dooWY68Hbot6C7',
                        database='fzy2025',
                        charset='utf8mb4'
                    )
                    with connection.cursor() as cursor:
                        cursor.execute("DELETE FROM sc WHERE cid=%s", (cid,))
                        cursor.execute("DELETE FROM tc WHERE cid=%s", (cid,))
                        cursor.execute("DELETE FROM clubs WHERE cid=%s", (cid,))
                    connection.commit()
                    table.removeRow(row)
                    QMessageBox.information(sub_window, "成功", f"社团编号为 {cid} 的社团已删除。")
                except pymysql.Error as e:
                    QMessageBox.critical(sub_window, "错误", f"数据库错误: {str(e)}")
                    if 'connection' in locals():
                        connection.rollback()
                finally:
                    if 'connection' in locals() and connection.open:
                        connection.close()

        del_btn.clicked.connect(delete_row)
        btn_layout.addWidget(del_btn)

        layout.addWidget(table)
        layout.addLayout(btn_layout)
        sub_window.setLayout(layout)
        sub_window.exec_()
    def edit_event_info(self):
        sub_window = QDialog(self)
        sub_window.setWindowTitle('编辑活动信息')
        sub_window.setFixedSize(900, 500)
        layout = QVBoxLayout()

        table = QTableWidget()
        table.setColumnCount(5)
        table.setHorizontalHeaderLabels(['活动编号', '活动名称', '地点', '所属社团编号', '日期'])

        try:
            connection = pymysql.connect(
                host='mysql5.sqlpub.com',
                port=3310,
                user='fzy50253',
                password='r8dooWY68Hbot6C7',
                database='fzy2025',
                charset='utf8mb4'
            )
            with connection.cursor() as cursor:
                cursor.execute("SELECT eid, name, place, cid, date FROM events")
                events = cursor.fetchall()
                table.setRowCount(len(events))

                for row, event in enumerate(events):
                    eid, name, place, cid, date = event
                    table.setItem(row, 0, QTableWidgetItem(str(eid)))
                    table.item(row, 0).setFlags(table.item(row, 0).flags() & ~Qt.ItemIsEditable)

                    table.setItem(row, 1, QTableWidgetItem(name))
                    table.setItem(row, 2, QTableWidgetItem(place))
                    table.setItem(row, 3, QTableWidgetItem(str(cid)))
                    table.setItem(row, 4, QTableWidgetItem(date.strftime("%Y-%m-%d")))

            connection.commit()

        except pymysql.Error as e:
            QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
            if 'connection' in locals():
                connection.rollback()
        finally:
            if 'connection' in locals() and connection.open:
                connection.close()

        table.setEditTriggers(QTableWidget.DoubleClicked)
        table.setSelectionBehavior(QTableWidget.SelectRows)
        table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

        btn_layout = QHBoxLayout()

        save_btn = QPushButton("保存修改")
        def save_changes():
            try:
                connection = pymysql.connect(
                    host='mysql5.sqlpub.com',
                    port=3310,
                    user='fzy50253',
                    password='r8dooWY68Hbot6C7',
                    database='fzy2025',
                    charset='utf8mb4'
                )
                with connection.cursor() as cursor:
                    for row in range(table.rowCount()):
                        eid = table.item(row, 0).text()
                        name = table.item(row, 1).text()
                        place = table.item(row, 2).text()
                        cid = table.item(row, 3).text()
                        date = table.item(row, 4).text()
                        if not all([eid, name, place, cid, date]):
                            continue
                        cursor.execute("""
                            UPDATE events SET name=%s, place=%s, cid=%s, date=%s WHERE eid=%s
                        """, (name, place, cid, date, eid))
                connection.commit()
                QMessageBox.information(self, "成功", "活动信息已保存。")
            except pymysql.Error as e:
                QMessageBox.critical(self, "错误", f"数据库错误: {str(e)}")
                if 'connection' in locals():
                    connection.rollback()
            finally:
                if 'connection' in locals() and connection.open:
                    connection.close()

        save_btn.clicked.connect(save_changes)
        btn_layout.addWidget(save_btn)

        del_btn = QPushButton("删除选中行")
        def delete_row():
            selected = table.selectedItems()
            if not selected:
                QMessageBox.warning(sub_window, "提示", "请选择要删除的活动行")
                return
            row = selected[0].row()
            eid = table.item(row, 0).text()
            ret = QMessageBox.question(sub_window, "确认删除", f"确认删除活动编号为 {eid} 的活动吗？", QMessageBox.Yes | QMessageBox.No)
            if ret == QMessageBox.Yes:
                try:
                    connection = pymysql.connect(
                        host='mysql5.sqlpub.com',
                        port=3310,
                        user='fzy50253',
                        password='r8dooWY68Hbot6C7',
                        database='fzy2025',
                        charset='utf8mb4'
                    )
                    with connection.cursor() as cursor:
                        cursor.execute("DELETE FROM events WHERE eid=%s", (eid,))
                    connection.commit()
                    table.removeRow(row)
                    QMessageBox.information(sub_window, "成功", f"活动编号为 {eid} 的活动已删除。")
                except pymysql.Error as e:
                    QMessageBox.critical(sub_window, "错误", f"数据库错误: {str(e)}")
                    if 'connection' in locals():
                        connection.rollback()
                finally:
                    if 'connection' in locals() and connection.open:
                        connection.close()

        del_btn.clicked.connect(delete_row)
        btn_layout.addWidget(del_btn)

        layout.addWidget(table)
        layout.addLayout(btn_layout)
        sub_window.setLayout(layout)
        sub_window.exec_()
if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())