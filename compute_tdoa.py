import csv

def read_timestamps(filename):
    timestamps = {}
    with open(filename, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            rid = int(row['receiver_id'])
            ts = int(row['timestamp'])
            if rid not in timestamps:
                timestamps[rid] = []
            timestamps[rid].append(ts)
    return timestamps

def compute_tdoa(ts_data):
    rec_ids = sorted(ts_data.keys())
    if len(rec_ids) < 2:
        print("Besoin d'au moins 2 récepteurs pour calculer le TDoA.")
        return

    print("Calcul TDoA entre les récepteurs :")
    for i in range(min(len(ts_data[rec_ids[0]]), len(ts_data[rec_ids[1]]))):
        t1 = ts_data[rec_ids[0]][i]
        t2 = ts_data[rec_ids[1]][i]
        tdoa = t2 - t1
        print(f"Mesure {i+1} : TDOA = {tdoa} unités d'horloge")

if __name__ == "__main__":
    filename = 'uwb_timestamps.csv'
    ts_data = read_timestamps(filename)
    compute_tdoa(ts_data)
