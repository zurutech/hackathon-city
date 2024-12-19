from model_data import ModelData, Constraint, Rotator, Option, AdjacencyOption
from itertools import product
from collections import defaultdict

BASE_PATH = "/Game/HackatonCity/actors/"
ALL_ANGLES = [0, 90, -90, 180]
HOR_ANGLES = [0, 180]
VER_ANGLES = [90, -90]
FOUR_WAY_LOT = ["city1.city1", "city5.city5"]
# FOUR_WAY_LOT = ["city1.city1"]
HOR_TWO_WAY_LOT = ["city4.city4", "city6.city6", "city7.city7"]
VER_TWO_WAY_LOT = ["city2.city2", "city0.city0"]

def model_data_matrix():
    model_dict = defaultdict(lambda: defaultdict(lambda: {
        "Front": [],
        "Back": [],
        "Right": [],
        "Left": []
    }))
    count = 0
    for lot1 in FOUR_WAY_LOT:
        for rot1 in ALL_ANGLES:
            count += 1
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
            count += 1
            for lot2 in FOUR_WAY_LOT:
                for rot2 in ALL_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
            for lot2 in HOR_TWO_WAY_LOT:
                for rot2 in HOR_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Front"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Back"].append((lot2, rot2))
            for lot2 in VER_TWO_WAY_LOT:
                for rot2 in VER_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
        for rot1 in VER_ANGLES:
            count += 1
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
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
    for lot1 in VER_TWO_WAY_LOT:
        for rot1 in HOR_ANGLES:
            count += 1
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
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
        for rot1 in VER_ANGLES:
            count += 1
            for lot2 in FOUR_WAY_LOT:
                for rot2 in ALL_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
            for lot2 in HOR_TWO_WAY_LOT:
                for rot2 in HOR_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Front"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Back"].append((lot2, rot2))
            for lot2 in VER_TWO_WAY_LOT:
                for rot2 in VER_ANGLES:
                    model_dict[lot1][rot1]["Left"].append((lot2, rot2))
                    model_dict[lot1][rot1]["Right"].append((lot2, rot2))
    # breakpoint()
    return (
        {
            key: dict(value)
            for key, value in model_dict.items()
        },
        count
    )

def generate_model_datas(model_dict, weight) -> ModelData:
    model_data = ModelData(
        Constraints=[
            Constraint(
                BaseObject=BASE_PATH + key,
                BaseRotator=Rotator(Yaw=rotation),
                Weight=weight,
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
    
    return model_data

def get_model_data() -> ModelData:
    model_dict, count = model_data_matrix()
    print(model_dict)
    model_data = generate_model_datas(model_dict=model_dict, weight=1/count)
    return model_data
