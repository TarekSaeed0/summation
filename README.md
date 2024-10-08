# Summation

<a href="https://github.com/TarekSaeed0/summation/pulse">
    <img alt="Last commit" src="https://img.shields.io/github/last-commit/TarekSaeed0/summation?style=for-the-badge&labelColor=%231e1e2e&color=%2394e2d5">
</a>
<a href="https://github.com/TarekSaeed0/summation/blob/main/LICENSE">
    <img alt="License" src="https://img.shields.io/github/license/TarekSaeed0/summation?style=for-the-badge&labelColor=%231e1e2e&color=%23cba6f7">
</a>
<a href="https://github.com/TarekSaeed0/summation/stargazers">
    <img alt="Repo stars" src="https://img.shields.io/github/stars/TarekSaeed0/summation?style=for-the-badge&labelColor=%231e1e2e&color=%23f9e2af">
</a>

_An easy-to-use command-line application for evaluating summations._

`summation` is a C program that takes a lower bound `n` and upper bound `m` and an expression `f(i)`,
and evaluates the summation of the summand `f(i)` from `n` to `m` inclusive.

# Usage

```sh
summation LOWER_BOUND UPPER_BOUND SUMMAND

```

## Examples

```sh
> summation 1 5 "1 + sin(i^2)"
5.07653
> summation 0 10 "1 / 2 ^ (i + 1)"
5.5
```
