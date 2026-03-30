# T2.3 and T2.4 Resolved

## T2.3: Basic Formulation with Risk Analysis = K

### Proposed Approach

For `K > 1`, the idea is to use the max-flow model for assignments as a feasibility test under reviewer failures.

The method is:

1. Build the original flow network exactly as in the basic formulation.
2. Compute the baseline max-flow.
3. If the baseline flow is smaller than the total number of required reviews, the original instance is already infeasible, so risk analysis stops.
4. Otherwise, generate all subsets of reviewers with size `K`.
5. For each subset, temporarily remove those reviewers from the graph.
6. Run max-flow again on the reduced network.
7. If the new flow is smaller than the required total number of reviews, then that subset is critical.

The final output is the list of all reviewer sets of size `K` whose simultaneous failure makes the assignment infeasible.

This is a direct and exact approach. It is based on the fact that max-flow already tells us whether the assignment constraints can still be satisfied after removing some reviewers.

### Why This Is Correct

In the flow model, a feasible assignment exists if and only if the maximum flow reaches:

`number_of_submissions * minimum_reviews_per_submission`

After removing a set of `K` reviewers, we rebuild the same network without those reviewer nodes. If the maximum flow no longer reaches the required total, then at least one submission cannot obtain enough reviews, so that reviewer set is indeed critical. If the flow still reaches the target, then a valid assignment still exists.

Therefore, testing every subset of size `K` guarantees that all critical reviewer sets are found.

### Complexity

Let:

- `R` be the number of reviewers
- `S` be the number of submissions
- `V` be the number of vertices in the flow graph
- `E` be the number of edges

The algorithm checks all reviewer subsets of size `K`, so there are:

`C(R, K)`

such subsets.

For each subset, we run max-flow once. Using Edmonds-Karp, each run costs:

`O(V * E^2)`

So the total time complexity is:

`O(C(R, K) * V * E^2)`

The space complexity is:

`O(V^2)`

because the flow network is stored as a capacity matrix.

This approach is exact, but expensive for large `K`, since the number of subsets grows combinatorially. It is therefore most suitable for small values of `K`.

### Nontrivial Example

Consider:

- 4 submissions
- each submission requires 2 reviews
- 6 reviewers
- `K = 2`

Suppose the compatibility is such that:

- submission 1 can be reviewed by reviewers 1, 2, 5
- submission 2 can be reviewed by reviewers 1, 2, 6
- submission 3 can be reviewed by reviewers 3, 4, 5
- submission 4 can be reviewed by reviewers 3, 4, 6

and each reviewer can review at most 2 submissions.

In the original instance, a valid assignment exists. However:

- if reviewers `{1,2}` fail, submissions 1 and 2 lose too much coverage
- if reviewers `{3,4}` fail, submissions 3 and 4 lose too much coverage

All other reviewer pairs still allow a feasible assignment.

### Corresponding Output

For this example, the output of the risk analysis could be:

```text
#Risk Analysis: 2
1, 2
3, 4
```

This output is nontrivial because:

- not every pair is critical
- more than one critical set exists
- the algorithm must test all pairs to be certain that no other critical combinations exist

This illustrates both the correctness of the method and the combinatorial source of its complexity.

## T2.4: General Formulation without Risk Analysis

### Proposed Approach

In the general formulation, both submissions and reviewers may have primary and secondary domains. If we only want any feasible assignment, we can still use a standard max-flow model with more compatibility edges. However, the statement asks for the more general case where an optimal assignment is sought. For that version, a better approach is to model the problem as a Min-Cost Max-Flow network.

The idea is:

1. Create a source node connected to each submission node with capacity equal to the minimum number of required reviews.
2. Connect each submission to compatible reviewers.
3. Connect each reviewer to the sink with capacity equal to the maximum number of reviews that reviewer can perform.
4. Associate a cost to each submission-reviewer edge according to the quality of the match.

For example:

- primary-primary match: cost `0`
- primary-secondary or secondary-primary match: cost `1`
- secondary-secondary match: cost `2`

Then we run a Min-Cost Max-Flow algorithm:

- first objective: assign all required reviews
- second objective: among all feasible assignments, minimize the total matching cost

This preserves the flow-based structure while allowing the algorithm to prefer stronger matches over weaker ones.

### Why This Is Appropriate

The basic max-flow model only answers whether enough compatible assignments exist. It does not distinguish between a very strong assignment and a weaker one if both are feasible.

In the general case, multiple feasible assignments may exist, and we may want to optimize quality, for example by:

- preferring primary-primary matches
- using secondary-domain matches only when needed
- minimizing reliance on weaker expertise combinations

Min-Cost Max-Flow naturally captures this optimization goal.

### Compatibility Rule

A reviewer is compatible with a submission if at least one of the following holds:

- reviewer primary = submission primary
- reviewer primary = submission secondary
- reviewer secondary = submission primary
- reviewer secondary = submission secondary

Every compatible pair becomes an edge in the network, with a cost that reflects the strength of the match.

### Complexity

Let:

- `S` be the number of submissions
- `R` be the number of reviewers
- `V` be the number of vertices
- `E` be the number of edges
- `F` be the total flow to send, that is, the total number of required reviews

Building the compatibility graph may require checking every submission-reviewer pair, so preprocessing takes:

`O(S * R)`

The complexity of Min-Cost Max-Flow depends on the implementation. Using a standard successive shortest augmenting path method with nonnegative costs, a common bound is:

`O(F * E * log V)`

for shortest-path computations with priority queues, plus the graph construction cost.

Since the general formulation introduces more compatibility edges than the basic one, `E` may become much larger. This is the main reason why the problem becomes more computationally demanding in practice.

### Comparison with the Basic Formulation

In the basic formulation:

- only a limited set of compatibility edges exists
- max-flow is enough to test feasibility

In the general formulation:

- many more compatibility edges may exist
- several feasible assignments may coexist
- optimization matters, not just feasibility

That is why a cost-based flow formulation is more suitable.

### Small Example

Suppose:

- submission A: primary `3`, secondary `4`
- reviewer 1: primary `3`, secondary none
- reviewer 2: primary `1`, secondary `4`
- reviewer 3: primary `4`, secondary `3`

Then:

- reviewer 1 has a primary-primary match with cost `0`
- reviewer 2 has a secondary-secondary style match with cost `2`
- reviewer 3 may match through either domain, depending on the scoring policy

If the submission needs 2 reviewers, the algorithm will prefer reviewer 1 first, and then choose the cheapest remaining compatible reviewer. This shows how the model uses secondary domains while still favoring stronger expertise matches whenever possible.

### Final Remark

So, a good answer for T2.4 is not just "add more edges and run max-flow". That is enough for feasibility, but not for the optimal general case suggested by the statement. A Min-Cost Max-Flow formulation is more appropriate because it handles both feasibility and optimization in a unified way.
