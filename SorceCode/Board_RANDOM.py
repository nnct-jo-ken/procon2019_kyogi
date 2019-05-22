###---パッケージのインポート---###
import random
import tkinter
###--------------------###

###---盤面の対称性のパターン決定---###
BOARD_MODE = random.randrange(1,4) # 1番:上下左右対称 2:上下のみ対称 3:左右のみ対称
###--------------------###

###---盤面の得点リスト生成---###
BOARD_X = random.randrange(10,21) #盤の横のマス数(10～20の間のランダムな整数値)
BOARD_Y = random.randrange(10,21) #盤の縦のマス数(10～20の間のランダムな整数値)
BOARD = [] #盤の点数のリスト(2次元)を生成する
SMALL_BOARD = [] #小盤の点数リスト

#小盤の大きさの決定
if BOARD_MODE == 1: #上下左右対称の時
    #小盤の横のマス数
    SMALL_BOARD_X = BOARD_X // 2 + BOARD_X % 2
    SMALL_AGENTS_X = SMALL_BOARD_X

    #小盤の縦のマス数
    SMALL_BOARD_Y = BOARD_Y // 2 + BOARD_Y % 2
    SMALL_AGENTS_Y = SMALL_BOARD_Y

elif BOARD_MODE == 2: #上下のみ対称の時
    #小盤の横のマス数
    SMALL_BOARD_X = BOARD_X
    SMALL_AGENTS_X = SMALL_BOARD_X
    
    #小盤の縦のマス数
    SMALL_BOARD_Y = BOARD_Y // 2 + BOARD_Y % 2
    SMALL_AGENTS_Y = BOARD_Y // 2

else: #上下のみ対称の時
    #小盤の横のマス数
    SMALL_BOARD_X = BOARD_X // 2 + BOARD_X % 2
    SMALL_AGENTS_X = BOARD_X // 2
    
    #小盤の縦のマス数
    SMALL_BOARD_Y = BOARD_Y
    SMALL_AGENTS_Y = SMALL_BOARD_Y

#リスト(盤)の大きさを決める(各要素の初期値はNone)
for i in range(BOARD_Y):
    BOARD.append([None for i in range(BOARD_X)])

#小リスト(盤)の大きさを決める(各要素の初期値はNone)
for i in range(SMALL_BOARD_Y):
    SMALL_BOARD.append([None for i in range(SMALL_BOARD_X)])

SUM_SCORE = -1 #各マスの点数の合計

#各マスの合計点が0以上になるように小リストに得点をランダム生成する
while SUM_SCORE < 0:
    SUM_SCORE = 0
    for i in range(SMALL_BOARD_Y):
        for j in range(SMALL_BOARD_X):
            POINT_SCORE = random.randrange(-16,17) #得点のランダム生成
            SMALL_BOARD[i][j] = POINT_SCORE #リストに得点代入       
            SUM_SCORE += POINT_SCORE #得点の合計

#盤の得点リスト生成
for i in range(SMALL_BOARD_Y):
    for j in range(SMALL_BOARD_X):
        BOARD[i][j] = SMALL_BOARD[i][j] #盤左上部のリストに代入
        if BOARD_MODE != 3: #左右対称の時以外
            BOARD[BOARD_Y - 1 - i][j] = SMALL_BOARD[i][j] #盤(左)下部のリストに代入
        if BOARD_MODE != 2: #上下対称の時以外
            BOARD[i][BOARD_X - 1 - j] = SMALL_BOARD[i][j] #盤右(上)部のリストに得点代入
        if BOARD_MODE == 1: #上下左右対称のとき
            BOARD[BOARD_Y - 1 - i][BOARD_X - 1 - j] = SMALL_BOARD[i][j] #盤右下部のリストに得点代入
if BOARD_MODE == 1:
    print(str(BOARD_MODE) + ":上下左右対称")
elif BOARD_MODE == 2:
    print(str(BOARD_MODE) + ":上下対称")
else:
    print(str(BOARD_MODE) + ":左右対称")
print(str(BOARD_Y) + "×" + str(BOARD_X)) #盤面のサイズ表示(デバッグ用)
###--------------------###

###---エージェントの座標リスト生成---###
MY_AGENTS_POSITION = [] #自分のエージェントの座標リスト
SMALL_MY_AGENTS_POSITION = [] #自分のエージェントの小盤面の座標リスト
ENEMY_AGENTS_POSITION = [] #敵のエージェントの座標リスト
SMALL_ENEMY_AGENTS_POSITION = [] #相手のエージェントの小盤面の座標リスト
if BOARD_MODE == 1: #上下左右対称の時
    SMALL_AGENTS = random.randrange(1,3) #小盤面の両チームそれぞれのエージェントの数
else: #その他
    SMALL_AGENTS = random.randrange(1,5) #小盤面の両チームそれぞれのエージェントの数
POSITIONS = [] #エージェントの座標重複防止

#小盤面のエージェントの座標リスト生成
#自分のエージェント
for i in range(SMALL_AGENTS):
    AGENT_X = random.randrange(SMALL_AGENTS_X) #ランダムx値生成
    AGENT_Y = random.randrange(SMALL_AGENTS_Y) #ランダムy値生成
    SMALL_MY_AGENTS_POSITION.append([AGENT_X, AGENT_Y])
#相手のエージェント
for i in range(SMALL_AGENTS):
    AGENT_X = random.randrange(SMALL_AGENTS_X)
    AGENT_Y = random.randrange(SMALL_AGENTS_Y)
    while [AGENT_X, AGENT_Y] == SMALL_MY_AGENTS_POSITION[i]:
        AGENT_X = random.randrange(SMALL_AGENTS_X)
        AGENT_Y = random.randrange(SMALL_AGENTS_Y)
    SMALL_ENEMY_AGENTS_POSITION.append([AGENT_X, AGENT_Y])

#左上のエージェントの座標リスト生成
for i in range(SMALL_AGENTS):
    MY_AGENTS_POSITION.append([SMALL_MY_AGENTS_POSITION[i][0], SMALL_MY_AGENTS_POSITION[i][1]]) #自分のエージェント
    ENEMY_AGENTS_POSITION.append([SMALL_ENEMY_AGENTS_POSITION[i][0], SMALL_ENEMY_AGENTS_POSITION[i][1]]) #相手のエージェント

#右下のエージェントの座標リスト生成
if BOARD_MODE == 1: #上下左右対称の時
    for i in range(SMALL_AGENTS):
        if (BOARD_X - 1 - MY_AGENTS_POSITION[i][0] != MY_AGENTS_POSITION[i][0]) and (BOARD_Y - 1 - MY_AGENTS_POSITION[i][1] != MY_AGENTS_POSITION[i][1]):
            MY_AGENTS_POSITION.append([BOARD_X - 1 - SMALL_MY_AGENTS_POSITION[i][0], BOARD_Y - 1 - SMALL_MY_AGENTS_POSITION[i][1]]) #自分のエージェント
        if (BOARD_X - 1 - ENEMY_AGENTS_POSITION[i][0] != ENEMY_AGENTS_POSITION[i][0]) and (BOARD_Y - 1 - ENEMY_AGENTS_POSITION[i][1] != ENEMY_AGENTS_POSITION[i][1]):
            ENEMY_AGENTS_POSITION.append([BOARD_X - 1 - SMALL_ENEMY_AGENTS_POSITION[i][0], BOARD_Y - 1 - SMALL_ENEMY_AGENTS_POSITION[i][1]]) #相手のエージェント

#左下,右上のエージェントの座標リスト生成
#自分のエージェント
for i in range(SMALL_AGENTS):
    if (BOARD_X - 1 - ENEMY_AGENTS_POSITION[i][0] != ENEMY_AGENTS_POSITION[i][0]) and (BOARD_MODE != 2):
        MY_AGENTS_POSITION.append([BOARD_X - 1 - ENEMY_AGENTS_POSITION[i][0], ENEMY_AGENTS_POSITION[i][1]])
    if (BOARD_Y - 1 - ENEMY_AGENTS_POSITION[i][1] != ENEMY_AGENTS_POSITION[i][1]) and (BOARD_MODE != 3):
        MY_AGENTS_POSITION.append([ENEMY_AGENTS_POSITION[i][0], BOARD_Y - 1 - ENEMY_AGENTS_POSITION[i][1]])

#相手のエージェント
for i in range(SMALL_AGENTS):
    if (BOARD_X - 1 - MY_AGENTS_POSITION[i][0] != MY_AGENTS_POSITION[i][0]) and (BOARD_MODE != 2):
        ENEMY_AGENTS_POSITION.append([BOARD_X - 1 - MY_AGENTS_POSITION[i][0], MY_AGENTS_POSITION[i][1]])
    if (BOARD_Y - 1 - MY_AGENTS_POSITION[i][1] != MY_AGENTS_POSITION[i][1]) and (BOARD_MODE != 3):
        ENEMY_AGENTS_POSITION.append([MY_AGENTS_POSITION[i][0], BOARD_Y - 1 - MY_AGENTS_POSITION[i][1]])

###--------------------###

###---盤面の表示---###
WIDTH = 30 * BOARD_X + 20
HEIGHT = 30 * BOARD_Y + 20
root = tkinter.Tk()
root.title(u"procon_kyogi") #ウインドウのタイトル指定
root.geometry("620x620")
canvas = tkinter.Canvas(root, width = WIDTH, height = HEIGHT)

for i in range(BOARD_X + 1):
    canvas.create_line(10 + 30 * i, 10 , 10 + 30 * i, HEIGHT - 10)
for i in range(BOARD_Y + 1):
    canvas.create_line(10, 10 + 30 * i , WIDTH - 10, 10 + 30 * i)

for i in range(BOARD_Y):
    for j in range(BOARD_X):
        canvas.create_text(25 + 30 * j, 25 + 30 * i, text = str(BOARD[i][j]))

for i in range(len(MY_AGENTS_POSITION)):
    canvas.create_oval(10 + 30 * MY_AGENTS_POSITION[i][0], 10 + 30 * MY_AGENTS_POSITION[i][1], 40 + 30 * MY_AGENTS_POSITION[i][0], 40 + 30 * MY_AGENTS_POSITION[i][1], fill = "#ff0000")
    canvas.create_oval(10 + 30 * ENEMY_AGENTS_POSITION[i][0], 10 + 30 * ENEMY_AGENTS_POSITION[i][1], 40 + 30 * ENEMY_AGENTS_POSITION[i][0], 40 + 30 * ENEMY_AGENTS_POSITION[i][1], fill = "#0000ff")

canvas.place(x = (620 - WIDTH) / 2, y = (620 - HEIGHT) / 2)

root.mainloop()
###--------------------###