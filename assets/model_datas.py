from model_data import ModelData, Constraint, Rotator, Option, AdjacencyOption
from itertools import product
from collections import defaultdict

BASE_PATH = "/Game/HackatonCity/actors/"
ALL_ANGLES = [0, 90, -90, 180]
HOR_ANGLES = [0, 180]
VER_ANGLES = [90, -90]
FOUR_WAY_LOT = ["city1.city1", "city5.city5"]
HOR_TWO_WAY_LOT = ["city4.city4", "city6.city6", "city7.city7"]
VER_TWO_WAY_LOT = ["city2.city2", "city0.city0"]

def model_data_matrix():
    model_dict = defaultdict(lambda: defaultdict(lambda: {
        "Front": [],
        "Back": [],
        "Right": [],
        "Left": []
    }))
    for lot1 in FOUR_WAY_LOT:
        for rot1 in ALL_ANGLES:
            for lot2 in FOUR_WAY_LOT:
                for rot2 in ALL_ANGLES:
                    model_dict[lot1][rot1]["Front"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Back"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
            for lot2 in HOR_TWO_WAY_LOT:
                for rot2 in HOR_ANGLES:
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                for rot2 in VER_ANGLES:
                    model_dict[lot1][rot1]["Front"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Back"].append((lot2, rot2))
            for lot2 in VER_TWO_WAY_LOT:
                for rot2 in HOR_ANGLES:
                    model_dict[lot1][rot1]["Front"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Back"].append((lot2, rot2))
                for rot2 in VER_ANGLES:
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
    for lot1 in HOR_TWO_WAY_LOT:
        for rot1 in HOR_ANGLES:
            for lot2 in FOUR_WAY_LOT:
                for rot2 in ALL_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
            for lot2 in HOR_TWO_WAY_LOT:
                for rot2 in HOR_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
            for lot2 in VER_TWO_WAY_LOT:
                for rot2 in VER_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
        for rot1 in VER_ANGLES:
            for lot2 in FOUR_WAY_LOT:
                for rot2 in ALL_ANGLES:
                    model_dict[lot1][rot1]["Front"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Back"].append((lot2, rot2))
            for lot2 in HOR_TWO_WAY_LOT:
                for rot2 in VER_ANGLES:
                    model_dict[lot1][rot1]["Front"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Back"].append((lot2, rot2))
            for lot2 in VER_TWO_WAY_LOT:
                for rot2 in HOR_ANGLES:
                    model_dict[lot1][rot1]["Front"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Back"].append((lot2, rot2))
    for lot1 in VER_TWO_WAY_LOT:
        for rot1 in HOR_ANGLES:
            for lot2 in FOUR_WAY_LOT:
                for rot2 in ALL_ANGLES:
                    model_dict[lot1][rot1]["Front"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Back"].append((lot2, rot2))
            for lot2 in HOR_TWO_WAY_LOT:
                for rot2 in VER_ANGLES:
                    model_dict[lot1][rot1]["Front"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Back"].append((lot2, rot2))
            for lot2 in VER_TWO_WAY_LOT:
                for rot2 in HOR_ANGLES:
                    model_dict[lot1][rot1]["Front"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Back"].append((lot2, rot2))
        for rot1 in VER_ANGLES:
            for lot2 in FOUR_WAY_LOT:
                for rot2 in ALL_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
            for lot2 in HOR_TWO_WAY_LOT:
                for rot2 in HOR_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
            for lot2 in VER_TWO_WAY_LOT:
                for rot2 in VER_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
    breakpoint()
    return {
        key: dict(value)
        for key, value in model_dict.items()
    }

def generate_model_datas(model_dict) -> ModelData:
    model_data = ModelData(
        Constraints=[
            Constraint(
                BaseObject=BASE_PATH + key,
                BaseRotator=Rotator(Yaw=rotation),
                AdjacencyToOptionsMap=[
                    AdjacencyOption(
                        direction=direction,
                        options=[
                            Option(
                                BaseObject=BASE_PATH + option[0],
                                BaseRotator=Rotator(Yaw=option[1])
                            )
                            for option in constraints[direction]
                        ]
                    )
                    for direction in constraints
                ]
            )
            for key, value in model_dict.items()
            for rotation, constraints in value.items()
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
    print(model_dict)
    model_data = generate_model_datas(model_dict=model_dict)
    return model_data
