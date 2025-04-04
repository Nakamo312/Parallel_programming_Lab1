import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import argparse
from matplotlib.font_manager import FontProperties

def plot_results(csv_file, output_image='matrix_performance.png'):
    """Plot performance results from CSV file."""

    try:
        df = pd.read_csv(csv_file)
    except FileNotFoundError:
        print(f"Error: File not found - {csv_file}")
        return
    except Exception as e:
        print(f"Error reading CSV file: {e}")
        return

    if 'Multiplication Time (s)' in df.columns:
        df['Time (ms)'] = df['Multiplication Time (s)'] * 1000
    elif 'Multiplication Time (ms)' in df.columns:
        df['Time (ms)'] = df['Multiplication Time (ms)']
    
    plt.figure(figsize=(12, 8))
    sns.set_style("whitegrid")
    
    plt.rcParams['font.family'] = 'DejaVu Sans'
    
    ax = sns.lineplot(
        x='Dimension', 
        y='Time (ms)', 
        data=df,
        marker='o',
        markersize=8,
        linewidth=2
    )
    
    ax.set_title('Matrix Multiplication Performance', fontsize=16, pad=20)
    ax.set_xlabel('Matrix Dimension (N x N)', fontsize=12)
    ax.set_ylabel('Execution Time (ms)', fontsize=12)
    ax.grid(True, which='both', linestyle='--', linewidth=0.5)
    
    for i, row in df.iterrows():
        validation_symbol = 'OK' if row['Validation Result'] == True else 'X'
        ax.annotate(
            validation_symbol,
            (row['Dimension'], row['Time (ms)']),
            textcoords="offset points",
            xytext=(0,10),
            ha='center',
            fontsize=10,
            color='green' if row['Validation Result'] == True else 'red'
        )
    
    plt.tight_layout()
    try:
        plt.savefig(output_image, dpi=300, bbox_inches='tight')
        print(f"Plot saved to: {output_image}")
    except Exception as e:
        print(f"Error saving plot: {e}")
    
    plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Plot matrix multiplication performance results.')
    parser.add_argument('--csv', default='matrix_results.csv', help='CSV file with results')
    parser.add_argument('--output', default='matrix_performance.png', help='Output image file')
    args = parser.parse_args()
    
    plot_results(args.csv, args.output)