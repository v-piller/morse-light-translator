import pandas as pd
import matplotlib.pyplot as plt

# Chargement du CSV
fichier_csv = '/home/mathi/sop2025/404ErrorNotFound/light_sensor.csv'
df = pd.read_csv(fichier_csv)

# Affichage des premières lignes pour vérification
print(df.head())

# Tracé du graphique
plt.figure(figsize=(10, 5))
plt.plot(df['index'], df['valeur'], marker='o', linestyle='-', color='blue')
plt.title('Mesures du capteur de lumière')
plt.xlabel('Index')
plt.ylabel('Valeur')
plt.grid(True)
plt.tight_layout()
plt.show()
