class ActInfo:
    # id : サーバーから受けとったidをそのまま使ってください
    # dx : -1 ～ 1
    # dy : -1 ～ 1
    # type : 行動の種類 0:停留、1:移動、2:タイル除去
    def __init__(self, id, dx, dy, type):
        self.id = id
        self.dx = dx
        self.dy = dy
        self.type = type