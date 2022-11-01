import argparse
import matplotlib.pyplot as plt
import math
import numpy as np
from scipy.stats import poisson


def calc_probability(string_size: int, max_num_occurrences: list, data_size: int, data_unit: str):
    # Get the multiple for the data size to determine size of data interval
    data_unit_mapping = {'kb': 10, 'mb': 20, 'gb': 30, 'tb': 40, 'pb': 50}
    interval_exp = data_unit_mapping[data_unit.lower()]

    # Only need to calculate probability through our upper limit of string occurrences
    max_num_occurrence = max(max_num_occurrences)
    poisson_pdf = calc_poisson_pmf(string_size, max_num_occurrence, data_size, interval_exp)
    probability = poisson_pdf*100

    # Determine total probability of each string occurring n-times
    probability_list = [poisson_pdf[mnm-1] for mnm in max_num_occurrences]
    probability_total = np.prod(probability_list) * 100

    X = list(range(1, max_num_occurrence + 1))
    fig, ax = plt.subplots()
    fig.set_size_inches(12.5, 9.5)
    ax.plot(X, probability, linestyle='-', marker="o")
    ax.set_xticks(X)
    ax.set_xlabel("Number of Occurrences")
    ax.set_ylabel("% Probability")
    title_string = f"Probability of String with length={string_size} occurring {max_num_occurrence} times\n" \
        f"in Data Set of Size {data_size}{data_unit.upper()}"
    plt.title(title_string)
    desc_text = f"Probability of {len(max_num_occurrences)} strings occurring " \
        f"({', '.join([str(mno) for mno in max_num_occurrences])}) times with length:{string_size} = {probability_total:#.3g}%"
    plt.figtext(0.5, 0.01, desc_text, ha="center", fontsize=18,
                bbox={"facecolor": "grey", "alpha": 0.5, "pad": 5})

    for x, y in zip(X, probability):
        ax.annotate(f"{y:#.3g}%", (x, y), fontsize=10, horizontalalignment='center', verticalalignment='bottom')
    plt.savefig(f"probability_s{string_size}_n{max_num_occurrence}_d{data_size}{data_unit.upper()}.png", dpi=200)


def calc_poisson_pmf(string_size: int, max_num_occurrences: int, data_size: int, interval_exp: int) -> list:
    # https://en.wikipedia.org/wiki/Poisson_distribution
    # S is an 8-bit string of length string_size
    # Rate of S = 1/(2**(8*string_size))
    # Currently only tracking the exponent to cancel with exponent for interval
    rate_exp = 8 * string_size
    # λ is the expected rate of occurrence for the string
    # λ = r*t, r:rate, t:interval
    # The interval is the size of the data, currently reduced to data_size*2**interval_exp
    # To help mitigate overflow, we take 2^(interval_exp - rate_exp) as they will cancel out
    lambda_rate = data_size * math.pow(2, interval_exp - rate_exp)
    # Take the Poisson distribution
    X = list(range(1, max_num_occurrences+1))
    poisson_pdf = poisson.pmf(X, lambda_rate)

    return poisson_pdf


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-s', type=int, help="Length of string S")
    parser.add_argument('-n', type=list, nargs='+', help="The number of occurrences of S")
    parser.add_argument('-d', type=int, help="The size of the data")
    parser.add_argument(
        '-u',
        type=str,
        help="The unit of the size of data, default is 'GB'.",
        default='GB',
        choices={'kb', 'KB', 'mb', 'MB', 'gb', 'GB', 'tb', 'TB', 'pb', 'PB'}
    )

    args = parser.parse_args()

    string_lengths = [int(''.join(s)) for s in args.n]
    calc_probability(args.s, string_lengths, args.d, args.u)


if __name__ == "__main__":
    main()
