import sys
import os

import json
import yaml


class RCSConfig:
    def __init__(self, argv):
        if len(argv) < 2:
            # 使用默认配置文件 (config.yaml)
            self.parameters = self.read_from_yaml()
        else:
            # 使用命令行参数指定配置文件

            # 检测文件名后缀
            if argv[1].endswith(".json"):
                self.parameters = self.read_from_json(argv[1])
            elif argv[1].endswith(".yaml"):
                self.parameters = self.read_from_yaml(argv[1])
            else:
                print("Error: Invalid file type. Please use .json or .yaml file.")
                sys.exit(1)

    def read_from_json(self, path="config.json"):

        # get current file path
        current_workspace_path = os.getcwd()
        print("current_workspace_path = \n", current_workspace_path)

        # 读取根目录的配置文件
        config_json_path = current_workspace_path + "/" + path
        print("config_json_path = \n", config_json_path)

        with open(config_json_path, "r", encoding="utf-8") as json_file:
            parameters = json.load(fp=json_file)

        # print json in beautiful format
        print("#################################")
        print("config.json is: ")
        print(json.dumps(parameters, indent=4, sort_keys=False))
        print("#################################")

        return parameters

    def read_from_yaml(self, path="config.yaml"):
        # get current file path
        current_workspace_path = os.getcwd()
        print("current_workspace_path = \n", current_workspace_path)

        # 读取根目录的配置文件
        config_yaml_path = current_workspace_path + "/" + path
        print("config_yaml_path = \n", config_yaml_path)

        with open(config_yaml_path, "r") as yaml_file:
            parameters = yaml.load(yaml_file, Loader=yaml.FullLoader)

        # print yaml in beautiful format
        print("#################################")
        print("config.yaml is: ")
        print(yaml.dump(parameters, indent=4, sort_keys=False))
        print("#################################")

        return parameters


# Singleton Object
gl_rcs_config = RCSConfig(sys.argv)
