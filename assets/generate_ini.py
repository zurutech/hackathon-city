import configparser
from pathlib import Path

from model_data import ModelData, Constraint, Rotator, Scale3D, Option, AdjacencyOption

DAFAULT_CONFIG_FILENAME = Path('../Config/DefaultGame.ini')
CONFIG_FILENAME = 'output.ini'

class CamelCaseConfigParser(configparser.ConfigParser):
    def optionxform(self, optionstr: str) -> str:
        return optionstr

def generate_config(model_data: ModelData) -> CamelCaseConfigParser:
    config = CamelCaseConfigParser()

    # read the default config file
    with open(DAFAULT_CONFIG_FILENAME, 'r') as configfile:
        config.read_file(configfile)

    # modify the modeldata section
    constraints_str = []
    for constraint in model_data.Constraints:
        constraint_str = f'    (\n        (\n            BaseObject="{constraint.BaseObject}",\n            BaseRotator=(\n                Pitch={constraint.BaseRotator.Pitch},\n                Yaw={constraint.BaseRotator.Yaw},\n                Roll={constraint.BaseRotator.Roll}\n            ),\n            BaseScale3D=(\n                X={constraint.BaseScale3D.X},\n                Y={constraint.BaseScale3D.Y},\n                Z={constraint.BaseScale3D.Z}\n            )\n        ),\n        (\n            AdjacencyToOptionsMap=('
        
        for adjacency in constraint.AdjacencyToOptionsMap:
            if adjacency.options:
                options_str = ', '.join([f'(\n                                BaseObject="{option.BaseObject}"\n                                BaseRotator=(\n                                    Pitch={option.BaseRotator.Pitch},\n                                    Yaw={option.BaseRotator.Yaw},\n                                    Roll={option.BaseRotator.Roll}\n                                ),\n                                BaseScale3D=(\n                                    X={option.BaseScale3D.X},\n                                    Y={option.BaseScale3D.Y},\n                                    Z={option.BaseScale3D.Z}\n                                )' for option in adjacency.options])
                constraint_str += f'\n                (\n                    {adjacency.direction}, (\n                        Options=(\n                            {options_str}\n                            )\n                        )\n                    )\n                ),'
            else:
                constraint_str += f'\n                (\n                    {adjacency.direction}, ()\n                ),'
        
        constraint_str = constraint_str.rstrip(',') + '\n            )\n        )\n    )'
        constraints_str.append(constraint_str)
    
    config['/Script/hackaton_city.HackatonCityDeveloperSettings'] = {
        'ModelData': f'(\nConstraints=(\n{",\n".join(constraints_str)}\n)\n)'
    }
    
    return config

def write_config(config: CamelCaseConfigParser, filename: str) -> None:
    with open(CONFIG_FILENAME, 'w') as configfile:
        config.write(configfile)

def main():
    model_data = ModelData(
        Constraints=[
            Constraint(
                BaseObject="/Engine/Functions/Engine_MaterialFunctions02/SupportFiles/1x1x1_Box_Pivot_-XYZ.1x1x1_Box_Pivot_-XYZ",
                BaseRotator=Rotator(Pitch=0.0, Yaw=0.0, Roll=0.0),
                BaseScale3D=Scale3D(X=1.0, Y=1.0, Z=1.0),
                AdjacencyToOptionsMap=[
                    AdjacencyOption(direction="Front", options=[Option(BaseObject="/Game/HackatonCity/low_wideA.low_wideA")]),
                    AdjacencyOption(direction="Back"),
                    AdjacencyOption(direction="Right"),
                    AdjacencyOption(direction="Left")
                ]
            ),
            Constraint(
                BaseObject="/Game/HackatonCity/small_buildingE.small_buildingE",
                AdjacencyToOptionsMap=[
                    AdjacencyOption(direction="Front", options=[Option(BaseObject="/Game/HackatonCity/large_buildingF.large_buildingF")]),
                    AdjacencyOption(direction="Back"),
                    AdjacencyOption(direction="Right"),
                    AdjacencyOption(direction="Left")
                ]
            )
        ]
    )
    config = generate_config(model_data)
    write_config(config=config, filename=CONFIG_FILENAME)

if __name__ == '__main__':
    main()