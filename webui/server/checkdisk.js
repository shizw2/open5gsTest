const { exec } = require('child_process');

exec('df -B 1 / | tail -n +2', (error, stdout, stderr) => {
    if (error) {
        console.error(`执行命令时出错: ${error}`);
        return;
    }
    if (stderr) {
        console.error(`命令 stderr: ${stderr}`);
        return;
    }
    
    const lines = stdout.split('\n');
    lines.forEach(line => {
        const columns = line.split(/\s+/);
        if (columns.length >= 6) { // 确保列数足够
            const total = parseInt(columns[1], 10);
            const used = parseInt(columns[2], 10);
            const available = parseInt(columns[3], 10);
            const usage = (used / total) * 100;
            
            console.log(`总空间: ${total}, 已用空间: ${used}, 可用空间: ${available}, 使用率: ${usage.toFixed(2)}%`);
        }
    });
});