from model_data import ModelData, Constraint, Rotator, Option, AdjacencyOption

BASE_PATH = "/Game/HackatonCity/actors/"

def model_data_matrix():
    model_dict = {
        "city1.city1": {
            "rotation": [0, 0, 0],
            "constraints": {
                "city1.city1": {
                    "Front": [("city1.city1", [0, 0, 0])],
                    "Back": [("city1.city1", [0, 0, 0])],
                    "Right": [("city1.city1", [0, 0, 0])],
                    "Left": [("city1.city1", [0, 0, 0])]
                }
            }
        }
    }
    return model_dict

def generate_model_datas(model_dict) -> ModelData:
    model_data = ModelData(
        Constraints=[
            Constraint(
                BaseObject=BASE_PATH + key,
                BaseRotator=Rotator(
                    Pitch=value["rotation"][0],
                    Yaw=value["rotation"][1],
                    Roll=value["rotation"][2]
                ),
                AdjacencyToOptionsMap=[
                    AdjacencyOption(
                        direction=direction,
                        options=[
                            Option(
                                BaseObject=BASE_PATH + option[0],
                                BaseRotator=Rotator(
                                    Pitch=option[1][0],
                                    Yaw=option[1][1],
                                    Roll=option[1][2]
                                )
                            )
                            for option in value["constraints"][key][direction]
                        ]
                    )
                    for direction in value["constraints"][key]
                ]
            )
            for key, value in model_dict.items()
        ]
    )
    model_data2 = ModelData(
        Constraints=[
            Constraint(
                BaseObject="/Engine/Functions/Engine_MaterialFunctions02/SupportFiles/1x1x1_Box_Pivot_-XYZ.1x1x1_Box_Pivot_-XYZ",
                BaseRotator=Rotator(Pitch=0.0, Yaw=0.0, Roll=0.0),
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
    return model_data

def get_model_data() -> ModelData:
    model_dict = model_data_matrix()
    model_data = generate_model_datas(model_dict=model_dict)
    return model_data
