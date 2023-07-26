import sys
import os
import json


class RCSConfig:

    def __init__(self, argv):
        if len(argv) < 2:
            # 使用默认配置文件
            self.parameters = self.read_from_json()
        else:
            # 使用命令行参数指定配置文件
            self.parameters = self.read_from_json(path=argv[1])

    def read_from_json(self, path="config.json"):

        # 判断文件是否存在
        if not os.path.exists(path):
            print("File not exist: ", path)
            return {}  # 返回空字典

        # get current file path
        current_workspace_path = os.getcwd()
        print("current_workspace_path = \n", current_workspace_path)

        current_file_path = os.path.abspath(__file__)
        print("current_file_path = \n", current_file_path)

        # current_folder_path = os.path.abspath(__file__).rsplit("/", 1)[0]
        current_folder_path = os.path.dirname(current_file_path)  # 获取当前文件路径的上一级目录
        print("current_folder_path = \n", current_folder_path)

        # 读取根目录的配置文件
        config_json_path = current_workspace_path + "/" + path
        parameters = json.load(fp=open(config_json_path, "r", encoding="utf-8"))
        print("config.json is: \n", parameters)

        # 根据根目录的配置文件配置项索引到子目录的详细配置文件
        if parameters["robot"]["mechanism"] == "":
            config_robot_json_path = current_folder_path + "/config_" \
                                     + parameters["robot"]["type"] \
                                     + ".json"
        else:
            config_robot_json_path = current_folder_path + "/config_" \
                                     + parameters["robot"]["type"] + "_" \
                                     + parameters["robot"]["mechanism"] \
                                     + ".json"
        parameters = json.load(fp=open(config_robot_json_path, "r", encoding="utf-8"))
        print("config.json is: \n", parameters)
        print("\n")

        return parameters


# Singleton Object
gl_rcs_config = RCSConfig(sys.argv)
