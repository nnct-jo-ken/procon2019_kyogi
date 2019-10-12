import requests
import json

TOKEN_STR = 'procon30_example_token'    # 仮トークン
HOST = '127.0.0.1'  # 仮
PORT = '57557'      # 仮

# 試合前情報取得
token = {'Authorization' : TOKEN_STR}
match = requests.get('http://' + HOST + ':' + PORT + '/matches', headers=token)

# debug
print(match)        # httpレスポンス表示 401:失敗 200:成功
print(match.text)   # jsonテキスト

match_json = json.loads(match.text)