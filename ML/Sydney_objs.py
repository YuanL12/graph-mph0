from torch_geometric.datasets import TUDataset
import numpy as np
from sklearn.metrics import roc_auc_score, roc_curve, auc
from sklearn.linear_model import LogisticRegression
from sklearn.svm import SVC
from sklearn.model_selection import train_test_split
import sys

sys.path.append("../build")
import graph_mph
from graph_mph import compute_absolute_MPH0_multi_critical_filtration, compute_grid_H0
from scipy.spatial import distance
import time
import os


# check its is a valid 2-filtration by looping over all edges
def valid_check_filtration(edges_unique, edge_values, node_values):
    for i, row in enumerate(edges_unique):
        u, v = row
        fe_1, fe_2 = edge_values[i]
        f_u_1, f_u_2 = node_values[u]
        f_v_1, f_v_2 = node_values[v]
        assert fe_1 >= f_u_1 and fe_1 >= f_v_1
        assert fe_2 >= f_u_2 and fe_2 >= f_v_2


def LR_classification(X_train, X_test, y_train, y_test):
    # Start Classfication now
    LR_model = LogisticRegression()
    LR_model.fit(X_train, y_train)
    # Calculate accuracy
    accuracy = LR_model.score(X_test, y_test)
    print(f"LogisticRegression accuracy = {accuracy}")


def SVM_classfication(X_train, X_test, y_train, y_test):
    # Initialize the SVM model
    svm_model = SVC()

    # Train the model
    svm_model.fit(X_train, y_train)

    # Calculate accuracy
    accuracy = svm_model.score(X_test, y_test)
    print(f"LogisticRegression accuracy = {accuracy}")


from sklearn.ensemble import RandomForestClassifier


def RandomForest_classification(X_train, X_test, y_train, y_test):
    # Initialize the Random Forest model
    rf_model = RandomForestClassifier()

    # Train the model
    rf_model.fit(X_train, y_train)

    # Calculate accuracy
    accuracy = rf_model.score(X_test, y_test)
    print(f"RandomForest accuracy = {accuracy}")


from sklearn.neighbors import KNeighborsClassifier


def KNN_classification(X_train, X_test, y_train, y_test):
    # Initialize the KNN model
    knn_model = KNeighborsClassifier()

    # Train the model
    knn_model.fit(X_train, y_train)

    # Calculate accuracy
    accuracy = knn_model.score(X_test, y_test)
    print(f"KNN accuracy = {accuracy}")


from sklearn.tree import DecisionTreeClassifier


def DecisionTree_classification(X_train, X_test, y_train, y_test):
    # Initialize the Decision Tree model
    dt_model = DecisionTreeClassifier()

    # Train the model
    dt_model.fit(X_train, y_train)

    # Calculate accuracy
    accuracy = dt_model.score(X_test, y_test)
    print(f"DecisionTree accuracy = {accuracy}")


from sklearn.naive_bayes import GaussianNB


def NaiveBayes_classification(X_train, X_test, y_train, y_test):
    # Initialize the Naive Bayes model
    nb_model = GaussianNB()

    # Train the model
    nb_model.fit(X_train, y_train)

    # Calculate accuracy
    accuracy = nb_model.score(X_test, y_test)
    print(f"NaiveBayes accuracy = {accuracy}")


from xgboost import XGBClassifier


def XGBoost_classification(X_train, X_test, y_train, y_test):
    # Initialize the XGBoost model
    xgb_model = XGBClassifier()

    # Train the model
    xgb_model.fit(X_train, y_train)

    # Calculate accuracy
    accuracy = xgb_model.score(X_test, y_test)
    print(f"XGBoost accuracy = {accuracy}")


import glob


def read_bin(file_name):
    names = ["t", "intensity", "id", "x", "y", "z", "azimuth", "range", "pid"]

    formats = [
        "int64",
        "uint8",
        "uint8",
        "float32",
        "float32",
        "float32",
        "float32",
        "float32",
        "int32",
    ]

    binType = np.dtype(dict(names=names, formats=formats))
    data = np.fromfile(file_name, binType)

    # 3D points, one per row
    P = np.vstack([data["x"], data["y"], data["z"]]).T

    return P


def load_sydney_urban_objects(
    labels_dict, folder_path="./sydney-urban-objects-dataset/objects"
):
    folders = glob.glob(folder_path)
    files = glob.glob(folders[0] + "/*")
    files_path = []
    labels = []
    for file in files:
        file_name = file.split("/")[-1]
        if file_name.split(".")[-1] == "bin":
            label = file_name.split(".")[0]
            labels.append(labels_dict[label])
            files_path.append(file)
    return files_path, labels


def greedy_landmarks_hausdorff(pdist, i0=0):
    # pdist: pairwise distance matrix
    inds = [i0]  # index order
    hds = []  # Hausdorff distance to full data set of indices up to that point

    d = pdist[i0, :].copy()

    while len(inds) < pdist.shape[1]:
        # get furthest point from landmark set
        i = np.argmax(d)
        hds.append(d[i])  # Hausdorff distance of set up to that point

        # insert new point
        inds.append(i)

        # update distances from set
        d = np.minimum(d, pdist[i, :])

    hds.append(0)
    return inds, hds


from scipy.spatial import distance


if __name__ == "__main__":
    # load dataset
    labels_dict = {
        "4wd": 0,
        "bench": 1,
        "bicycle": 2,
        "biker": 3,
        "building": 4,
        "bus": 5,
        "car": 6,
        "cyclist": 7,
        "excavator": 8,
        "pedestrian": 9,
        "pillar": 10,
        "pole": 11,
        "post": 12,
        "scooter": 13,
        "ticket_machine": 14,
        "traffic_lights": 15,
        "traffic_sign": 16,
        "trailer": 17,
        "trash": 18,
        "tree": 19,
        "truck": 20,
        "trunk": 21,
        "umbrella": 22,
        "ute": 23,
        "van": 24,
        "vegetation": 25,
    }
    files_path_list, labels = load_sydney_urban_objects(labels_dict)
    # find the classes that appear more than 10 times
    unique, counts = np.unique(labels, return_counts=True)
    labels_over_10_times = [u for u, c in zip(unique, counts) if c >= 10]
    # create new labels for them, new_labels[i] is the new label for classes_over_10_times[i]
    new_labels = [i for i in range(len(labels_over_10_times))]

    start_time = time.time()
    H0_grid_vals = []
    sample_size = 50
    ys = []
    # labels = labels[:20]
    # for file_path in files_path_list[:20]:
    for i, file_path in enumerate(files_path_list):
        # skip the classes that appear less than 10 times
        if labels[i] not in labels_over_10_times:
            continue
        ys.append(new_labels[labels_over_10_times.index(labels[i])])

        # read the point cloud data
        points_np = read_bin(file_path)

        # downsample the point cloud to 100 points
        pd_matrix = distance.squareform(distance.pdist(points_np))
        sub_sample_inds, hds = greedy_landmarks_hausdorff(pd_matrix)
        points_np = points_np[sub_sample_inds[:sample_size]]

        # compute the pairwise distance matrix
        n = points_np.shape[0]
        pd_mat = distance.squareform(distance.pdist(points_np))

        # compute the absolute persistence homology in dimension 0
        betti0s, betti1s, betti2s = compute_absolute_MPH0_multi_critical_filtration(
            pd_mat
        )

        # compute the grid of H_0 values
        x_range, y_range = [0, np.max(pd_mat)], [-n, 0]
        grid_size = 100
        H0_grid = compute_grid_H0(
            grid_size, x_range, y_range, betti0s, betti1s, betti2s
        )

        # mask the values with low frequency <= 1%
        mesh_size = H0_grid.size
        unique, counts = np.unique(H0_grid, return_counts=True)
        # print("Frequency of each value in H_0 values:")
        noise_values = []  # low frequency values
        for u, c in zip(unique, counts):
            # print(f"{u} : {c} times")
            if c <= 0.01 * mesh_size:
                noise_values.append(u)

        # mask the entries in H0_values of its frequency <= 10 as -1
        H0_grid_masked = np.where(H0_grid >= min(noise_values), -1, H0_grid)

        # save the masked H0 values into a file
        bin_file_name = file_path.split("/")[-1][:-4]

        # create a folder to save, if not exist
        if not os.path.exists(
            f"./sydney-urban-objects-dataset/H0_grids_sample_{sample_size}"
        ):
            os.makedirs(f"./sydney-urban-objects-dataset/H0_grids_sample_{sample_size}")

        np.save(
            f"./sydney-urban-objects-dataset/H0_grids_sample_{sample_size}/{bin_file_name}.npy",
            H0_grid_masked,
        )
        H0_grid_vals.append(H0_grid_masked)

    imgs_array = np.array([img.flatten() for img in H0_grid_vals])

    time_taken = time.time() - start_time
    print(f"Compute H0 values done, takes {time_taken:4f} seconds")

    # Start Classfication now
    print("=============================================================")
    print("Start Classification")
    Xs = imgs_array
    X_train, X_test, y_train, y_test = train_test_split(
        Xs, ys, test_size=0.30, random_state=42
    )

    LR_classification(X_train, X_test, y_train, y_test)
    SVM_classfication(X_train, X_test, y_train, y_test)
    RandomForest_classification(X_train, X_test, y_train, y_test)
    KNN_classification(X_train, X_test, y_train, y_test)
    DecisionTree_classification(X_train, X_test, y_train, y_test)
    NaiveBayes_classification(X_train, X_test, y_train, y_test)
    XGBoost_classification(X_train, X_test, y_train, y_test)
