import json
from actinfo import ActInfo

def make_json(act_list):
    dic_list = []
    for i in act_list:
        dic_list.append({
            "agentID": i.id,
            "type": i.type,
            "dx": i.dx,
            "dy": i.dy
        })

    return json.dumps({"actions": dic_list})

# テスト用
if __name__=="__main__":
    act_info_list = []
    act_info_list.append(ActInfo(1, 1, 1, "move"))
    act_info_list.append(ActInfo(2, 0, -1, "stay"))
    act_info_list.append(ActInfo(3, 0, 0, "remove"))

    json_str = make_json(act_info_list)
    print(json_str)