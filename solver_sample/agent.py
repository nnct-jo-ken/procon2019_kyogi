# 自チームは１、敵チームは２
# idは固定ではない
class Agent:
    def __init__(self, agent_id, x, y, team, done_butting, done_bad_act):
        self.x = x                          # int 0 ~ 盤面の幅 - 1
        self.y = y                          # int 0 ~ 盤面の高さ - 1
        self.agent_id = agent_id            # int 1試合の中で共通
        self.team = team                    # int 1(味方) or 2(敵)
        self.done_butting = done_butting    # bool 前のターンにバッティングを起こしたか
        self.done_bad_act = done_bad_act    # bool 無効な手を打ったか