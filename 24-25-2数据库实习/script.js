// 等待DOM加载完成
document.addEventListener('DOMContentLoaded', function() {
    // 获取DOM元素
    const navLinks = document.querySelectorAll('nav ul li a');
    const repairForm = document.querySelector('.repair-form form');
    const statusCards = document.querySelectorAll('.status-card');
    const actionButtons = document.querySelectorAll('.action-buttons .btn');
    
    // 1. 导航菜单激活状态切换
    navLinks.forEach(link => {
        link.addEventListener('click', function(e) {
            e.preventDefault();
            navLinks.forEach(l => l.classList.remove('active'));
            this.classList.add('active');
            
            // 根据点击的链接显示不同的内容区域
            const target = this.getAttribute('href');
            if (target === '#') return;
            
            document.querySelectorAll('main > section').forEach(section => {
                section.style.display = 'none';
            });
            
            document.querySelector(target).style.display = 'block';
        });
    });
    
    // 2. 表单提交处理
    if (repairForm) {
        repairForm.addEventListener('submit', function(e) {
            e.preventDefault();
            
            // 获取表单数据
            const formData = {
                studentId: document.getElementById('student-id').value,
                name: document.getElementById('name').value,
                building: document.getElementById('building').value,
                room: document.getElementById('room').value,
                repairType: document.getElementById('repair-type').value,
                description: document.getElementById('description').value,
                contact: document.getElementById('contact').value
            };
            
            // 简单验证
            if (!validateForm(formData)) {
                return;
            }
            
            // 模拟提交到服务器
            submitRepair(formData);
        });
    }
    
    // 3. 状态卡片点击事件
    statusCards.forEach(card => {
        card.addEventListener('click', function(e) {
            if (e.target.tagName === 'A') return;
            
            const status = this.querySelector('h3').textContent;
            alert(`即将显示${status}的报修列表`);
            // 实际应用中这里应该跳转到对应状态的报修列表页面
        });
    });
    
    // 4. 快速操作按钮点击事件
    actionButtons.forEach(button => {
        button.addEventListener('click', function(e) {
            e.preventDefault();
            
            if (this.classList.contains('primary')) {
                // 滚动到报修表单
                document.querySelector('.repair-form').scrollIntoView({
                    behavior: 'smooth'
                });
            } else if (this.classList.contains('secondary')) {
                // 显示报修记录
                alert('即将显示报修记录');
                // 实际应用中这里应该跳转到报修记录页面
            }
        });
    });
    
    // 5. 初始化显示首页内容
    document.querySelector('.welcome-section').style.display = 'block';
    document.querySelector('.repair-form').style.display = 'none';
});

// 表单验证函数
function validateForm(formData) {
    if (!formData.studentId) {
        alert('请输入学号');
        return false;
    }
    
    if (!formData.name) {
        alert('请输入姓名');
        return false;
    }
    
    if (!formData.building) {
        alert('请选择宿舍楼');
        return false;
    }
    
    if (!formData.room) {
        alert('请输入房间号');
        return false;
    }
    
    if (!formData.repairType) {
        alert('请选择报修类型');
        return false;
    }
    
    if (!formData.description) {
        alert('请输入问题描述');
        return false;
    }
    
    if (!formData.contact) {
        alert('请输入联系电话');
        return false;
    }
    
    return true;
}

// 模拟提交报修数据
function submitRepair(formData) {
    // 这里应该是AJAX请求到服务器
    console.log('提交报修数据:', formData);
    
    // 模拟异步请求
    setTimeout(() => {
        alert(`报修提交成功！\n维修单号: ${Math.floor(Math.random() * 1000000)}`);
        
        // 重置表单
        document.querySelector('.repair-form form').reset();
        
        // 更新状态卡片计数
        updateStatusCounts();
    }, 1000);
}

// 更新状态计数
function updateStatusCounts() {
    // 模拟从服务器获取最新数据
    const newCounts = {
        pending: Math.floor(Math.random() * 10),
        inProgress: Math.floor(Math.random() * 5),
        completed: Math.floor(Math.random() * 20)
    };
    
    document.querySelector('.status-card.pending .count').textContent = newCounts.pending;
    document.querySelector('.status-card.in-progress .count').textContent = newCounts.inProgress;
    document.querySelector('.status-card.completed .count').textContent = newCounts.completed;
}