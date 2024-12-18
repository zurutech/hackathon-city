import configparser
from pathlib import Path

from model_datas import get_model_data
from model_data import ModelData

DEFAULT_CONFIG_FILENAME = Path('../Config/DefaultGame.ini')
CONFIG_FILENAME = 'output.ini'
CONFIG_FILENAME = DEFAULT_CONFIG_FILENAME

class CamelCaseConfigParser(configparser.ConfigParser):
    def optionxform(self, optionstr: str) -> str:
        return optionstr

def generate_config(model_data: ModelData) -> CamelCaseConfigParser:
    config = CamelCaseConfigParser()

    # read the default config file
    with open(DEFAULT_CONFIG_FILENAME, 'r') as configfile:
        config.read_file(configfile)

    # modify the modeldata section
    constraints_str = []
    for constraint in model_data.Constraints:
        constraint_str = f'    (\n        (\n            BaseObject="{constraint.BaseObject}",\n            BaseRotator=(\n                Pitch={float(constraint.BaseRotator.Pitch)},\n                Yaw={float(constraint.BaseRotator.Yaw)},\n                Roll={float(constraint.BaseRotator.Roll)}\n            ),\n            BaseScale3D=(\n                X={constraint.BaseScale3D.X},\n                Y={constraint.BaseScale3D.Y},\n                Z={constraint.BaseScale3D.Z}\n            ),\n            Weight={constraint.Weight},\n        ),\n        (\n            AdjacencyToOptionsMap=('
        
        for adjacency in constraint.AdjacencyToOptionsMap:
            if adjacency.options:
                options_str = ', '.join([f'(\n                                BaseObject="{option.BaseObject}",\n                                BaseRotator=(\n                                    Pitch={float(option.BaseRotator.Pitch)},\n                                    Yaw={float(option.BaseRotator.Yaw)},\n                                    Roll={float(option.BaseRotator.Roll)}\n                                ),\n                                BaseScale3D=(\n                                    X={option.BaseScale3D.X},\n                                    Y={option.BaseScale3D.Y},\n                                    Z={option.BaseScale3D.Z}\n                                )' for option in adjacency.options])
                constraint_str += f'\n                (\n                    {adjacency.direction}, (\n                        Options=(\n                            {options_str}\n                            )\n                        )\n                    )\n                ),'
            else:
                constraint_str += f'\n                (\n                    {adjacency.direction}, ()\n                ),'
        
        constraint_str = constraint_str.rstrip(',') + '\n            )\n        )\n    )'
        constraints_str.append(constraint_str)
    
    model_data_str = f'(\nTileSize=7000.000000,\nConstraints=(\n{",\n".join(constraints_str)}\n)\n)'
    model_data_str = model_data_str.replace(" ", "").replace("\n", "")

    base_model_str = "/Game/HackatonCity/DataModel/DS_WFCM_HackatonCity.DS_WFCM_HackatonCity"
    
    config['/Script/hackaton_city.HackatonCityDeveloperSettings'] = {
        'BaseModel': base_model_str,
        'ModelData': model_data_str
    }
    
    return config

def write_config(config: CamelCaseConfigParser, filename: str) -> None:
    with open(CONFIG_FILENAME, 'w') as configfile:
        config.write(configfile)

def main():
    model_data = get_model_data()
    config = generate_config(model_data)
    write_config(config=config, filename=CONFIG_FILENAME)

if __name__ == '__main__':
    main()