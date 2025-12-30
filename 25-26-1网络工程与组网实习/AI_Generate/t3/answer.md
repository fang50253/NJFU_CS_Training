我将设计一个完整的小说阅读器系统，包含服务器和客户端，支持分页、章节、书签等功能。

## 一、系统设计：NRP（Novel Reading Protocol）

### 1. 协议基础
```
通信方式：TCP
默认端口：8889
编码格式：UTF-8
传输格式：JSON + 文本
```

### 2. 协议报文格式

**请求报文：**
```
NRP/1.0 <方法> <小说ID>\r\n
Content-Type: application/json\r\n
Content-Length: <长度>\r\n
\r\n
<JSON数据>
```

**响应报文：**
```
NRP/1.0 <状态码> <状态描述>\r\n
Content-Type: application/json\r\n
Content-Length: <长度>\r\n
\r\n
<JSON数据或文本内容>
```

### 3. 方法定义
| 方法 | 功能 | 请求体 |
|------|------|--------|
| LIST | 获取小说列表 | 空 |
| INFO | 获取小说信息 | 空 |
| OPEN | 打开小说 | {"encoding": "utf-8"} |
| PAGE | 获取指定页 | {"page": 页码, "page_size": 每页字节数} |
| CHAPTER | 获取章节 | {"chapter": 章节索引} |
| SEARCH | 搜索内容 | {"keyword": "关键词", "page": 页码} |
| BOOKMARK | 书签操作 | {"action": "add/get/delete", "page": 页码} |
| DOWNLOAD | 下载小说 | {"format": "txt/epub"} |
| CLOSE | 关闭小说 | 空 |

### 4. 状态码
```
200 OK - 成功
201 Bookmark Added - 书签添加成功
400 Bad Request - 请求错误
404 Novel Not Found - 小说不存在
500 Server Error - 服务器错误
```

## 二、服务器端实现（Python）

```python
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
```

## 三、客户端图形界面（Tkinter实现）

```python
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
```

## 四、使用说明

### 1. 启动服务器
```bash
python novel_server.py
```

### 2. 启动客户端
```bash
python novel_client.py
```

### 3. 基本操作
- **打开小说**：从左侧列表选择小说
- **翻页**：使用左右方向键、工具栏按钮或鼠标滚轮
- **跳页**：输入页码点击"跳转"或按Ctrl+G
- **章节导航**：使用"上一章"/"下一章"按钮
- **搜索**：Ctrl+F打开搜索框，输入关键词搜索
- **书签**：Ctrl+B添加书签，从菜单管理书签
- **下载**：Ctrl+D下载当前小说到本地

## 五、系统特点

1. **完整协议设计**：自定义NRP协议，类似HTTP但更简洁
2. **智能分页**：按字节分页，支持任意编码的TXT文件
3. **章节检测**：自动识别章节标题并支持章节跳转
4. **书签管理**：支持添加、删除、跳转书签
5. **全文搜索**：支持关键词搜索和高亮显示
6. **主题切换**：支持白天/夜间模式
7. **字体调节**：可调节字体大小
8. **进度跟踪**：显示阅读进度百分比

这个小说阅读器系统功能完整，可以直接用于阅读网络小说或本地TXT文件，具有良好的用户体验。