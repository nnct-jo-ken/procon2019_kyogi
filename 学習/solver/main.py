# 自作モジュール
from solver import Solver

# スレッド作成
# ポート7755(味方)、又は7756(敵)で接続
my_solver = Solver(7755)
enemy_solver = Solver(7756)

while True:
    
    my_solver.run()
    enemy_solver.run()