import sqlite3
import json
import os
from datetime import datetime
import threading

class MailDatabase:
    """邮件数据库管理"""
    def __init__(self, db_path='./mail_client.db'):
        self.db_path = db_path
        self.lock = threading.Lock()
        self._init_database()
    
    def _init_database(self):
        """初始化数据库"""
        with self.lock:
            conn = sqlite3.connect(self.db_path)
            cursor = conn.cursor()
            
            # 创建邮件表
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS mails (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    mail_id TEXT UNIQUE,
                    from_addr TEXT,
                    from_name TEXT,
                    to_addr TEXT,
                    to_name TEXT,
                    subject TEXT,
                    content_text TEXT,
                    content_html TEXT,
                    date DATETIME,
                    size INTEGER,
                    has_attachments BOOLEAN,
                    read BOOLEAN DEFAULT 0,
                    starred BOOLEAN DEFAULT 0,
                    folder TEXT DEFAULT 'inbox',
                    account TEXT,
                    raw_data TEXT,
                    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
                )
            ''')
            
            # 创建附件表
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS attachments (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    mail_id INTEGER,
                    filename TEXT,
                    filepath TEXT,
                    size INTEGER,
                    content_type TEXT,
                    downloaded BOOLEAN DEFAULT 0,
                    FOREIGN KEY (mail_id) REFERENCES mails (id)
                )
            ''')
            
            # 创建账户表
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS accounts (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    email TEXT UNIQUE,
                    username TEXT,
                    password TEXT,
                    smtp_host TEXT,
                    smtp_port INTEGER,
                    smtp_ssl BOOLEAN,
                    pop3_host TEXT,
                    pop3_port INTEGER,
                    pop3_ssl BOOLEAN,
                    signature TEXT,
                    last_sync DATETIME,
                    active BOOLEAN DEFAULT 1,
                    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
                )
            ''')
            
            # 创建文件夹表
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS folders (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    name TEXT,
                    account_id INTEGER,
                    type TEXT DEFAULT 'user',
                    unread_count INTEGER DEFAULT 0,
                    total_count INTEGER DEFAULT 0,
                    FOREIGN KEY (account_id) REFERENCES accounts (id)
                )
            ''')
            
            # 创建索引
            cursor.execute('CREATE INDEX IF NOT EXISTS idx_mails_date ON mails(date)')
            cursor.execute('CREATE INDEX IF NOT EXISTS idx_mails_folder ON mails(folder)')
            cursor.execute('CREATE INDEX IF NOT EXISTS idx_mails_account ON mails(account)')
            cursor.execute('CREATE INDEX IF NOT EXISTS idx_mails_read ON mails(read)')
            
            conn.commit()
            conn.close()
    
    def save_mail(self, mail_data, account_email, folder='inbox'):
        """保存邮件到数据库"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                # 检查是否已存在
                cursor.execute(
                    'SELECT id FROM mails WHERE mail_id = ? AND account = ?',
                    (mail_data.get('id'), account_email)
                )
                existing = cursor.fetchone()
                
                if existing:
                    # 更新现有邮件
                    cursor.execute('''
                        UPDATE mails SET 
                            from_addr = ?, from_name = ?,
                            to_addr = ?, to_name = ?,
                            subject = ?, content_text = ?,
                            content_html = ?, date = ?,
                            size = ?, has_attachments = ?,
                            folder = ?, updated_at = CURRENT_TIMESTAMP
                        WHERE id = ?
                    ''', (
                        mail_data.get('from'),
                        mail_data.get('from_name'),
                        mail_data.get('to'),
                        mail_data.get('to_name'),
                        mail_data.get('subject'),
                        mail_data.get('text', ''),
                        mail_data.get('html', ''),
                        mail_data.get('date'),
                        mail_data.get('size', 0),
                        1 if mail_data.get('attachments') else 0,
                        folder,
                        existing[0]
                    ))
                    mail_db_id = existing[0]
                else:
                    # 插入新邮件
                    cursor.execute('''
                        INSERT INTO mails (
                            mail_id, from_addr, from_name, to_addr, to_name,
                            subject, content_text, content_html, date, size,
                            has_attachments, folder, account, raw_data
                        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                    ''', (
                        mail_data.get('id'),
                        mail_data.get('from'),
                        mail_data.get('from_name'),
                        mail_data.get('to'),
                        mail_data.get('to_name'),
                        mail_data.get('subject'),
                        mail_data.get('text', ''),
                        mail_data.get('html', ''),
                        mail_data.get('date'),
                        mail_data.get('size', 0),
                        1 if mail_data.get('attachments') else 0,
                        folder,
                        account_email,
                        json.dumps(mail_data) if mail_data else ''
                    ))
                    mail_db_id = cursor.lastrowid
                
                # 保存附件信息
                if mail_data.get('attachments'):
                    for attachment in mail_data['attachments']:
                        cursor.execute('''
                            INSERT OR REPLACE INTO attachments 
                            (mail_id, filename, filepath, size, content_type, downloaded)
                            VALUES (?, ?, ?, ?, ?, ?)
                        ''', (
                            mail_db_id,
                            attachment.get('filename'),
                            attachment.get('path', ''),
                            attachment.get('size', 0),
                            attachment.get('content_type', 'application/octet-stream'),
                            1 if attachment.get('path') else 0
                        ))
                
                conn.commit()
                conn.close()
                
                return mail_db_id
                
            except Exception as e:
                print(f"保存邮件失败: {str(e)}")
                return None
    
    def get_mails(self, account_email, folder='inbox', limit=50, offset=0, 
                  unread_only=False, starred_only=False):
        """获取邮件列表"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                conn.row_factory = sqlite3.Row
                cursor = conn.cursor()
                
                query = '''
                    SELECT * FROM mails 
                    WHERE account = ? AND folder = ?
                '''
                params = [account_email, folder]
                
                if unread_only:
                    query += ' AND read = 0'
                if starred_only:
                    query += ' AND starred = 1'
                
                query += ' ORDER BY date DESC LIMIT ? OFFSET ?'
                params.extend([limit, offset])
                
                cursor.execute(query, params)
                rows = cursor.fetchall()
                
                mails = []
                for row in rows:
                    mail = dict(row)
                    # 转换日期
                    if mail['date']:
                        try:
                            mail['date'] = datetime.fromisoformat(mail['date'])
                        except:
                            pass
                    mails.append(mail)
                
                conn.close()
                return mails
                
            except Exception as e:
                print(f"获取邮件列表失败: {str(e)}")
                return []
    
    def get_mail(self, mail_db_id):
        """获取单封邮件"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                conn.row_factory = sqlite3.Row
                cursor = conn.cursor()
                
                cursor.execute('SELECT * FROM mails WHERE id = ?', (mail_db_id,))
                row = cursor.fetchone()
                
                if row:
                    mail = dict(row)
                    
                    # 获取附件
                    cursor.execute(
                        'SELECT * FROM attachments WHERE mail_id = ?',
                        (mail_db_id,)
                    )
                    attachments = [dict(a) for a in cursor.fetchall()]
                    mail['attachments'] = attachments
                    
                    conn.close()
                    return mail
                
                conn.close()
                return None
                
            except Exception as e:
                print(f"获取邮件失败: {str(e)}")
                return None
    
    def mark_as_read(self, mail_db_id, read=True):
        """标记邮件已读/未读"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                cursor.execute(
                    'UPDATE mails SET read = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?',
                    (1 if read else 0, mail_db_id)
                )
                
                conn.commit()
                conn.close()
                return True
                
            except Exception as e:
                print(f"标记邮件失败: {str(e)}")
                return False
    
    def star_mail(self, mail_db_id, starred=True):
        """星标邮件"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                cursor.execute(
                    'UPDATE mails SET starred = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?',
                    (1 if starred else 0, mail_db_id)
                )
                
                conn.commit()
                conn.close()
                return True
                
            except Exception as e:
                print(f"星标邮件失败: {str(e)}")
                return False
    
    def move_mail(self, mail_db_id, folder):
        """移动邮件到其他文件夹"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                cursor.execute(
                    'UPDATE mails SET folder = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?',
                    (folder, mail_db_id)
                )
                
                conn.commit()
                conn.close()
                return True
                
            except Exception as e:
                print(f"移动邮件失败: {str(e)}")
                return False
    
    def delete_mail(self, mail_db_id):
        """删除邮件"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                # 先删除附件记录
                cursor.execute('DELETE FROM attachments WHERE mail_id = ?', (mail_db_id,))
                
                # 删除邮件
                cursor.execute('DELETE FROM mails WHERE id = ?', (mail_db_id,))
                
                conn.commit()
                conn.close()
                return True
                
            except Exception as e:
                print(f"删除邮件失败: {str(e)}")
                return False
    
    def save_account(self, account_data):
        """保存账户配置"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                cursor = conn.cursor()
                
                cursor.execute('''
                    INSERT OR REPLACE INTO accounts 
                    (email, username, password, smtp_host, smtp_port, smtp_ssl,
                     pop3_host, pop3_port, pop3_ssl, signature, last_sync)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                ''', (
                    account_data['email'],
                    account_data['username'],
                    account_data['password'],
                    account_data.get('smtp_host', 'smtp.163.com'),
                    account_data.get('smtp_port', 25),
                    account_data.get('smtp_ssl', False),
                    account_data.get('pop3_host', 'pop.163.com'),
                    account_data.get('pop3_port', 110),
                    account_data.get('pop3_ssl', True),
                    account_data.get('signature', ''),
                    datetime.now().isoformat()
                ))
                
                conn.commit()
                conn.close()
                return True
                
            except Exception as e:
                print(f"保存账户失败: {str(e)}")
                return False
    
    def get_accounts(self):
        """获取所有账户"""
        with self.lock:
            try:
                conn = sqlite3.connect(self.db_path)
                conn.row_factory = sqlite3.Row
                cursor = conn.cursor()
                
                cursor.execute('SELECT * FROM accounts WHERE active = 1')
                accounts = [dict(a) for a in cursor.fetchall()]
                
                conn.close()
                return accounts
                
            except Exception as e:
                print(f"获取账户失败: {str(e)}")
                return []