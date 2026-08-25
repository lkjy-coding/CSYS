1.0.0

- \-

2.0.0

- 新增`dir`、`ver`、`chamod`、`file`（`file read`、`file change`）命令
- 修改`time`的显示格式为`YYYY-MM-DD HH:MM:SS`
- 删除`help`运行后显示内容中多余的\符号
- 新增错误命令提示
- 修复选择模式后需要再次Enter才能显示操作行的问题
- 支持使用`u`和`l`选择模式
- 优化显示
- 新增运行模式提示

3.0.0

- 新增`stopwatch`（`stopwatch start`、`stopwatch stop`、`stopwatch lap`、`stopwatch reset`、`stopwatch status`）、`move`、`copy`、`uptime`、`debug`命令
- 支持使用`?`快速启动`help`
- 修复了以下问题：
  - `permission`赋值问题

3.1.0

**Only on AC Git.**

4.0.0

- 新增了`:wq`、`settings`命令
- 修复了以下问题：
  - `fs::exists(dst_path)`（`move`&`copy`）处描述文本`Y`大小写错误的问题
  - `settings`界面无法正常退出的问题
