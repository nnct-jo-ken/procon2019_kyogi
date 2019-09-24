<<<<<<< HEAD:solver/main.py
# 自作モジュール
from solver import Solver

# スレッド作成
# ポート7755(味方)、又は7756(敵)で接続
my_solver = Solver(7755)
# enemy_solver = Solver(7756)

while True:
    my_solver.run()
=======
# 自作モジュール
from solver import Solver

# スレッド作成
# ポート7755(味方)、又は7756(敵)で接続
my_solver = Solver(7755)
# enemy_solver = Solver(7756)

while True:
    my_solver.run()
>>>>>>> e440c8c44c9d7d8e1dcabe301dba4933b3b5ea28:solver/main.py
    # enemy_solver.run()