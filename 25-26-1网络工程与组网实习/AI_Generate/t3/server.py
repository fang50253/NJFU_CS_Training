import json
import socket
import threading
import os
import re
from datetime import datetime
import mimetypes

class NovelServer:
    def __init__(self, host='0.0.0.0', port=8889):
        self.host = host
        self.port = port
        self.novels_dir = './novels'
        self.bookmarks_dir = './bookmarks'
        
        # 确保目录存在
        os.makedirs(self.novels_dir, exist_ok=True)
        os.makedirs(self.bookmarks_dir, exist_ok=True)
        
        # 加载小说索引
        self.novels_index = self.load_novels_index()
        self.active_sessions = {}  # 会话管理
    
    def start(self):
        """启动服务器"""
        server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind((self.host, self.port))
        server.listen(5)
        print(f"小说阅读器服务器启动在 {self.host}:{self.port}")
        print(f"小说目录: {os.path.abspath(self.novels_dir)}")
        
        while True:
            client, address = server.accept()
            session_id = f"{address[0]}:{address[1]}:{datetime.now().timestamp()}"
            self.active_sessions[session_id] = {
                'current_novel': None,
                'current_position': 0,
                'bookmarks': [],
                'page_size': 2000,  # 默认每页2000字节
                'last_active': datetime.now()
            }
            
            thread = threading.Thread(target=self.handle_client, 
                                    args=(client, address, session_id))
            thread.start()
    
    def handle_client(self, client, address, session_id):
        """处理客户端连接"""
        try:
            while True:
                # 接收请求
                try:
                    request_data = client.recv(8192).decode('utf-8')
                except:
                    break
                    
                if not request_data:
                    break
                
                # 更新活动时间
                self.active_sessions[session_id]['last_active'] = datetime.now()
                
                # 处理请求
                response = self.process_request(request_data, session_id)
                client.send(response.encode('utf-8'))
                
        except Exception as e:
            print(f"客户端 {address} 错误: {str(e)}")
        finally:
            # 清理会话
            if session_id in self.active_sessions:
                # 保存书签
                self.save_bookmarks(session_id)
                del self.active_sessions[session_id]
            client.close()
            print(f"客户端 {address} 断开连接")
    
    def process_request(self, request_data, session_id):
        """处理请求"""
        try:
            # 解析请求行和头部
            lines = request_data.split('\r\n')
            if len(lines) < 4:
                return self.create_response(400, "Bad Request")
            
            # 解析请求行
            request_line = lines[0]
            parts = request_line.split()
            if len(parts) < 3:
                return self.create_response(400, "Bad Request")
            
            protocol, method, novel_id = parts[0], parts[1], parts[2]
            
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
                try:
                    body_json = json.loads(body) if body.strip() else {}
                except:
                    body_json = {}
            else:
                body_json = {}
            
            # 处理不同方法
            if method == 'LIST':
                return self.handle_list()
            elif method == 'INFO':
                return self.handle_info(novel_id)
            elif method == 'OPEN':
                return self.handle_open(novel_id, session_id, body_json)
            elif method == 'PAGE':
                return self.handle_page(novel_id, session_id, body_json)
            elif method == 'CHAPTER':
                return self.handle_chapter(novel_id, session_id, body_json)
            elif method == 'SEARCH':
                return self.handle_search(novel_id, session_id, body_json)
            elif method == 'BOOKMARK':
                return self.handle_bookmark(novel_id, session_id, body_json)
            elif method == 'DOWNLOAD':
                return self.handle_download(novel_id, session_id)
            elif method == 'CLOSE':
                return self.handle_close(novel_id, session_id)
            else:
                return self.create_response(400, "Method Not Allowed")
                
        except Exception as e:
            return self.create_response(500, f"Server Error: {str(e)}")
    
    def handle_list(self):
        """获取小说列表"""
        novels = []
        for novel_id, info in self.novels_index.items():
            novel_info = {
                'id': novel_id,
                'title': info.get('title', '未知标题'),
                'author': info.get('author', '未知作者'),
                'size': info.get('size', 0),
                'chapters': info.get('chapters', 0),
                'last_modified': info.get('last_modified', '')
            }
            novels.append(novel_info)
        
        response_data = {
            'count': len(novels),
            'novels': novels
        }
        
        return self.create_response(200, "OK", response_data)
    
    def handle_info(self, novel_id):
        """获取小说信息"""
        if novel_id not in self.novels_index:
            return self.create_response(404, "Novel Not Found")
        
        info = self.novels_index[novel_id].copy()
        
        # 计算总页数
        page_size = 2000  # 默认值
        total_pages = (info['size'] // page_size) + (1 if info['size'] % page_size > 0 else 0)
        info['total_pages'] = total_pages
        
        return self.create_response(200, "OK", info)
    
    def handle_open(self, novel_id, session_id, data):
        """打开小说"""
        if novel_id not in self.novels_index:
            return self.create_response(404, "Novel Not Found")
        
        # 更新会话
        self.active_sessions[session_id]['current_novel'] = novel_id
        self.active_sessions[session_id]['current_position'] = 0
        
        # 加载书签
        self.load_bookmarks(session_id, novel_id)
        
        # 设置页面大小
        page_size = data.get('page_size', 2000)
        self.active_sessions[session_id]['page_size'] = page_size
        
        # 获取小说信息
        novel_info = self.novels_index[novel_id].copy()
        novel_path = os.path.join(self.novels_dir, novel_id)
        
        # 读取第一章内容（预览）
        try:
            with open(novel_path, 'r', encoding='utf-8') as f:
                preview = f.read(min(1000, novel_info['size']))
                novel_info['preview'] = preview
        except:
            novel_info['preview'] = ""
        
        return self.create_response(200, "OK", novel_info)
    
    def handle_page(self, novel_id, session_id, data):
        """获取指定页"""
        session = self.active_sessions.get(session_id)
        if not session or session['current_novel'] != novel_id:
            return self.create_response(400, "Novel Not Open")
        
        novel_path = os.path.join(self.novels_dir, novel_id)
        if not os.path.exists(novel_path):
            return self.create_response(404, "Novel Not Found")
        
        # 获取页码
        page = data.get('page', 1)
        page_size = data.get('page_size', session['page_size'])
        
        # 计算字节位置
        start_pos = (page - 1) * page_size
        end_pos = start_pos + page_size
        
        try:
            with open(novel_path, 'r', encoding='utf-8') as f:
                # 读取指定范围的内容
                f.seek(start_pos)
                content = f.read(page_size)
                
                # 获取文件总大小
                f.seek(0, 2)
                total_size = f.tell()
                
                # 计算总页数
                total_pages = (total_size // page_size) + (1 if total_size % page_size > 0 else 0)
                
                # 获取前后页内容（用于平滑滚动）
                prev_start = max(0, start_pos - page_size)
                next_end = min(total_size, end_pos + page_size)
                
                f.seek(prev_start)
                context_content = f.read(next_end - prev_start)
                
                response_data = {
                    'page': page,
                    'total_pages': total_pages,
                    'content': content,
                    'context': context_content,
                    'start_pos': start_pos,
                    'end_pos': end_pos,
                    'total_size': total_size,
                    'has_prev': page > 1,
                    'has_next': page < total_pages
                }
                
                # 更新当前位置
                session['current_position'] = start_pos
                
                return self.create_response(200, "OK", response_data)
                
        except Exception as e:
            return self.create_response(500, f"Read Error: {str(e)}")
    
    def handle_chapter(self, novel_id, session_id, data):
        """获取章节"""
        session = self.active_sessions.get(session_id)
        if not session or session['current_novel'] != novel_id:
            return self.create_response(400, "Novel Not Open")
        
        novel_path = os.path.join(self.novels_dir, novel_id)
        if not os.path.exists(novel_path):
            return self.create_response(404, "Novel Not Found")
        
        # 获取章节索引
        chapter_idx = data.get('chapter', 0)
        chapters = self.novels_index[novel_id].get('chapter_positions', [])
        
        if chapter_idx >= len(chapters):
            return self.create_response(400, "Chapter Not Found")
        
        # 获取章节位置
        chapter_info = chapters[chapter_idx]
        start_pos = chapter_info['position']
        
        # 计算章节结束位置（下一个章节开始或文件结尾）
        if chapter_idx + 1 < len(chapters):
            end_pos = chapters[chapter_idx + 1]['position']
        else:
            end_pos = self.novels_index[novel_id]['size']
        
        try:
            with open(novel_path, 'r', encoding='utf-8') as f:
                f.seek(start_pos)
                content = f.read(end_pos - start_pos)
                
                # 计算页码
                page_size = session['page_size']
                page = (start_pos // page_size) + 1
                total_pages = (self.novels_index[novel_id]['size'] // page_size) + 1
                
                response_data = {
                    'chapter': chapter_idx,
                    'title': chapter_info['title'],
                    'content': content,
                    'page': page,
                    'total_pages': total_pages,
                    'start_pos': start_pos,
                    'end_pos': end_pos,
                    'has_prev': chapter_idx > 0,
                    'has_next': chapter_idx + 1 < len(chapters)
                }
                
                # 更新当前位置
                session['current_position'] = start_pos
                
                return self.create_response(200, "OK", response_data)
                
        except Exception as e:
            return self.create_response(500, f"Read Error: {str(e)}")
    
    def handle_search(self, novel_id, session_id, data):
        """搜索内容"""
        session = self.active_sessions.get(session_id)
        if not session or session['current_novel'] != novel_id:
            return self.create_response(400, "Novel Not Open")
        
        novel_path = os.path.join(self.novels_dir, novel_id)
        if not os.path.exists(novel_path):
            return self.create_response(404, "Novel Not Found")
        
        keyword = data.get('keyword', '')
        page = data.get('page', 1)
        page_size = 10  # 每页显示10个结果
        
        if not keyword:
            return self.create_response(400, "Keyword Required")
        
        try:
            with open(novel_path, 'r', encoding='utf-8') as f:
                content = f.read()
                
                # 执行搜索（不区分大小写）
                results = []
                pattern = re.compile(re.escape(keyword), re.IGNORECASE)
                
                for match in pattern.finditer(content):
                    start = max(0, match.start() - 50)
                    end = min(len(content), match.end() + 50)
                    
                    # 提取上下文
                    context = content[start:end]
                    # 高亮关键词
                    highlighted = context.replace(
                        match.group(), 
                        f"<span class='highlight'>{match.group()}</span>"
                    )
                    
                    results.append({
                        'position': match.start(),
                        'preview': highlighted,
                        'page': (match.start() // session['page_size']) + 1
                    })
                
                # 分页
                total_results = len(results)
                total_pages = (total_results // page_size) + (1 if total_results % page_size > 0 else 0)
                start_idx = (page - 1) * page_size
                end_idx = min(start_idx + page_size, total_results)
                
                response_data = {
                    'keyword': keyword,
                    'total_results': total_results,
                    'page': page,
                    'total_pages': total_pages,
                    'results': results[start_idx:end_idx]
                }
                
                return self.create_response(200, "OK", response_data)
                
        except Exception as e:
            return self.create_response(500, f"Search Error: {str(e)}")
    
    def handle_bookmark(self, novel_id, session_id, data):
        """书签操作"""
        session = self.active_sessions.get(session_id)
        if not session or session['current_novel'] != novel_id:
            return self.create_response(400, "Novel Not Open")
        
        action = data.get('action', 'get')
        
        if action == 'add':
            page = data.get('page', 1)
            note = data.get('note', '')
            
            # 计算位置
            position = (page - 1) * session['page_size']
            
            bookmark = {
                'page': page,
                'position': position,
                'note': note,
                'timestamp': datetime.now().isoformat()
            }
            
            # 添加到书签列表
            session['bookmarks'].append(bookmark)
            session['bookmarks'].sort(key=lambda x: x['page'])
            
            # 保存到文件
            self.save_bookmarks(session_id)
            
            return self.create_response(201, "Bookmark Added", bookmark)
            
        elif action == 'get':
            return self.create_response(200, "OK", {'bookmarks': session['bookmarks']})
            
        elif action == 'delete':
            page = data.get('page')
            if page:
                # 删除指定页的书签
                session['bookmarks'] = [b for b in session['bookmarks'] if b['page'] != page]
                self.save_bookmarks(session_id)
                return self.create_response(200, "Bookmark Deleted")
            else:
                return self.create_response(400, "Page Required")
        else:
            return self.create_response(400, "Invalid Action")
    
    def handle_download(self, novel_id, session_id):
        """下载小说"""
        session = self.active_sessions.get(session_id)
        if not session or session['current_novel'] != novel_id:
            return self.create_response(400, "Novel Not Open")
        
        novel_path = os.path.join(self.novels_dir, novel_id)
        if not os.path.exists(novel_path):
            return self.create_response(404, "Novel Not Found")
        
        try:
            with open(novel_path, 'r', encoding='utf-8') as f:
                content = f.read()
                
                # 创建响应（这里简化处理，实际应该分块发送大文件）
                response = f"NRP/1.0 200 OK\r\n"
                response += f"Content-Type: text/plain; charset=utf-8\r\n"
                response += f"Content-Disposition: attachment; filename=\"{novel_id}\"\r\n"
                response += f"Content-Length: {len(content.encode('utf-8'))}\r\n"
                response += f"\r\n"
                response += content
                
                return response
                
        except Exception as e:
            return self.create_response(500, f"Download Error: {str(e)}")
    
    def handle_close(self, novel_id, session_id):
        """关闭小说"""
        session = self.active_sessions.get(session_id)
        if session and session['current_novel'] == novel_id:
            # 保存书签
            self.save_bookmarks(session_id)
            
            # 清空会话中的小说信息
            session['current_novel'] = None
            session['current_position'] = 0
            session['bookmarks'] = []
            
            return self.create_response(200, "Novel Closed")
        else:
            return self.create_response(400, "Novel Not Open")
    
    def load_novels_index(self):
        """加载小说索引"""
        novels_index = {}
        
        if not os.path.exists(self.novels_dir):
            return novels_index
        
        for filename in os.listdir(self.novels_dir):
            filepath = os.path.join(self.novels_dir, filename)
            
            if os.path.isfile(filepath) and filename.endswith('.txt'):
                try:
                    stat = os.stat(filepath)
                    
                    # 读取文件信息
                    with open(filepath, 'r', encoding='utf-8') as f:
                        # 读取第一行作为标题（如果有）
                        first_line = f.readline().strip()
                        title = first_line if first_line else filename[:-4]
                        
                        # 尝试检测章节
                        f.seek(0)
                        content = f.read()
                        
                        # 查找章节标题（简单的正则匹配）
                        chapter_pattern = re.compile(r'^第[零一二三四五六七八九十百千\d]+[章节回]\s*.+$', re.MULTILINE)
                        chapters = []
                        
                        for match in chapter_pattern.finditer(content):
                            chapters.append({
                                'title': match.group().strip(),
                                'position': match.start()
                            })
                    
                    novels_index[filename] = {
                        'title': title,
                        'filename': filename,
                        'size': stat.st_size,
                        'last_modified': datetime.fromtimestamp(stat.st_mtime).isoformat(),
                        'chapters': len(chapters),
                        'chapter_positions': chapters
                    }
                    
                except Exception as e:
                    print(f"加载小说 {filename} 失败: {str(e)}")
        
        return novels_index
    
    def load_bookmarks(self, session_id, novel_id):
        """加载书签"""
        bookmark_file = os.path.join(self.bookmarks_dir, f"{session_id}_{novel_id}.json")
        
        if os.path.exists(bookmark_file):
            try:
                with open(bookmark_file, 'r', encoding='utf-8') as f:
                    bookmarks = json.load(f)
                    self.active_sessions[session_id]['bookmarks'] = bookmarks
            except:
                self.active_sessions[session_id]['bookmarks'] = []
        else:
            self.active_sessions[session_id]['bookmarks'] = []
    
    def save_bookmarks(self, session_id):
        """保存书签"""
        session = self.active_sessions.get(session_id)
        if not session or not session['current_novel']:
            return
        
        bookmark_file = os.path.join(self.bookmarks_dir, f"{session_id}_{session['current_novel']}.json")
        
        try:
            with open(bookmark_file, 'w', encoding='utf-8') as f:
                json.dump(session['bookmarks'], f, ensure_ascii=False, indent=2)
        except:
            pass
    
    def create_response(self, status_code, status_text, data=None):
        """创建响应"""
        if data is None:
            data = {}
        
        body = json.dumps(data, ensure_ascii=False)
        response = f"NRP/1.0 {status_code} {status_text}\r\n"
        response += f"Content-Type: application/json; charset=utf-8\r\n"
        response += f"Content-Length: {len(body.encode('utf-8'))}\r\n"
        response += f"\r\n"
        response += body
        
        return response

if __name__ == "__main__":
    # 如果没有小说，创建一个示例小说
    novels_dir = './novels'
    os.makedirs(novels_dir, exist_ok=True)
    
    sample_novel = os.path.join(novels_dir, '三国演义.txt')
    if not os.path.exists(sample_novel):
        sample_content = """第一回 宴桃园豪杰三结义 斩黄巾英雄首立功

滚滚长江东逝水，浪花淘尽英雄。是非成败转头空。青山依旧在，几度夕阳红。
白发渔樵江渚上，惯看秋月春风。一壶浊酒喜相逢。古今多少事，都付笑谈中。

话说天下大势，分久必合，合久必分。周末七国分争，并入于秦。及秦灭之后，楚、汉分争，又并入于汉。汉朝自高祖斩白蛇而起义，一统天下，后来光武中兴，传至献帝，遂分为三国。推其致乱之由，殆始于桓、灵二帝。桓帝禁锢善类，崇信宦官。及桓帝崩，灵帝即位，大将军窦武、太傅陈蕃共相辅佐。时有宦官曹节等弄权，窦武、陈蕃谋诛之，机事不密，反为所害，中涓自此愈横。

建宁二年四月望日，帝御温德殿。方升座，殿角狂风骤起。只见一条大青蛇，从梁上飞将下来，蟠于椅上。帝惊倒，左右急救入宫，百官俱奔避。须臾，蛇不见了。忽然大雷大雨，加以冰雹，落到半夜方止，坏却房屋无数。建宁四年二月，洛阳地震；又海水泛溢，沿海居民，尽被大浪卷入海中。光和元年，雌鸡化雄。六月朔，黑气十余丈，飞入温德殿中。秋七月，有虹现于玉堂；五原山岸，尽皆崩裂。种种不祥，非止一端。

第二回 张翼德怒鞭督邮 何国舅谋诛宦竖

且说董卓字仲颖，陇西临洮人也，官拜河东太守，自来骄傲。当日怠慢了玄德，张飞性发，便欲杀之。玄德与关公急止之曰："他是朝廷命官，岂可擅杀？"飞曰："若不杀这厮，反要在他部下听令，其实不甘！二兄要便住在此，我自投别处去也！"玄德曰："我三人义同生死，岂可相离？不若都投别处去便了。"飞曰："若如此，稍解吾恨。"

于是三人连夜引军来投朱儁。儁待之甚厚，合兵一处，进讨张宝。是时曹操自跟皇甫嵩讨张梁，大战于曲阳。这里朱儁进攻张宝。张宝引贼众八九万，屯于山后。儁令玄德为其先锋，与贼对敌。张宝遣副将高升出马搦战，玄德使张飞击之。飞纵马挺矛，与升交战，不数合，刺升落马。玄德麾军直冲过去。张宝就马上披发仗剑，作起妖法。只见风雷大作，一股黑气，从天而降：黑气中似有无限人马杀来。玄德连忙回军，军中大乱。

第三回 议温明董卓叱丁原 馈金珠李肃说吕布

却说陈宫临欲下手杀曹操，忽转念曰："我为国家跟他到此，杀之不义。不若弃而他往。"插剑上马，不等天明，自投东郡去了。操觉，不见陈宫，寻思："此人见我说了这两句，疑我不仁，弃我而去；吾当急行，不可久留。"遂连夜到陈留，寻见父亲，备说前事；欲散家资，招募义兵。父言："资少恐不成事。此间有孝廉卫弘，疏财仗义，其家巨富；若得相助，事可图矣。"

操置酒张筵，拜请卫弘到家，告曰："今汉室无主，董卓专权，欺君害民，天下切齿。操欲力扶社稷，恨力不足。公乃忠义之士，敢求相助！"卫弘曰："吾有是心久矣，恨未遇英雄耳。既孟德有大志，愿将家资相助。"操大喜；于是先发矫诏，驰报各道，然后招集义兵，竖起招兵白旗一面，上书"忠义"二字。不数日间，应募之士，如雨骈集。

（示例内容，实际应包含完整小说）"""
        
        with open(sample_novel, 'w', encoding='utf-8') as f:
            f.write(sample_content)
    
    server = NovelServer(port=8889)
    server.start()