# 自チームは１、敵チームは２
# idは固定ではない
class Agent:
    def __init__(self, agent_id, x, y, team):
        self.x = x
        self.y = y
        self.agent_id = agent_id
        self.team = team
        self.type = 0 # 0 停留, 1 移動, 2 除去
        self.dx = 0 # -1 ~ 1
        self.dy = 0 # -1 ~ 1