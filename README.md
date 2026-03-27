# project-da

## T2.3: Risk Analysis with K > 1 Reviewers

### What's the Problem?
So the idea here is: what if multiple reviewers drop out at the same time? Like, if 2 or 3 reviewers suddenly can't do their reviews, would the whole assignment still work? We need to check if we can still get enough reviews for each paper even if K reviewers fail us.

### How We'd Solve It

**The Basic Idea: Just Keep Testing Combinations**

1. First, we get the baseline assignment that actually works (using the max-flow we already built)
2. Then we basically try removing different groups of K reviewers
3. For each group we remove, we run max-flow again and see if it still works
4. We keep track of which groups would actually break things
5. Once we find them all, we report back which combinations of reviewers would cause problems

### How Long Would This Take?

- **Time**: O(C(R,K) × V×E²) 
  - Basically, for every combination of K reviewers we could pick from R total reviewers, we run the whole max-flow algorithm
  - If we have 8 reviewers and K=2, that's 28 different pairs to check
  - For K=3, it gets worse fast (56 combinations), so this only really works for small K values
  
- **Space**: O(V²) - just need the matrix to run the algorithm each time

### A Real Example
Let's say we have 5 papers that each need 2 reviews, 8 reviewers total, and we want to check if any pair of reviewers could break everything:
- We'd try all 28 possible pairs
- For each pair, temporarily remove them and check if we can still assign 10 total reviews
- We'd end up with a list like: "If reviewers 2 and 5 both quit, we're stuck"

### Implementation Tips
- Use combinations (like `next_combination()`) to generate all the pairs/groups
- You could speed this up by stopping early if you find the assignment already fails
- Maybe cache the baseline results so you don't redo them every time

---

## T2.4: General Formulation with Secondary Domains

### What's the Problem?
Right now we only care about primary domains - like, a reviewer with primary computer science can only review computer science papers. But realistically, reviewers have secondary expertise too. So a reviewer who's mainly in AI but also knows machine learning should be able to review papers in either field. We need to make the matching more flexible.

### How We'd Solve It

**The Main Idea: More Connections Between Reviewers and Papers**

1. When building the flow network, instead of just checking if primary domains match, we check ANY domain
2. A reviewer can be connected to a paper if:
   - Their primary expertise matches the paper's primary domain, OR
   - Their primary expertise matches the paper's secondary domain, OR
   - Their secondary expertise matches the paper's primary domain, OR
   - Their secondary expertise matches the paper's secondary domain
3. Once we've got more edges drawn in, we run Edmonds-Karp just like before
4. The output would show which domain actually matched (like "reviewer's secondary matched paper's primary")

### How Long Would This Take?

- **Time**: O(V×E²) 
  - Looks the same as before, but E is actually bigger now because there are more possible connections
  - Worst case is if everyone can review everything - then we'd have way more edges
  - Still roughly O((S×R)² × (S+R)) but with more edges in the graph
  
- **Space**: O((S+R)²) - same matrix size as before

### What's Different from T2.1?

- **T2.1**: Only edges where primary matches primary (fewer connections)
- **T2.4**: Edges where ANY domain matches ANY domain (way more connections)
- **Result**: More flexibility means more reviewers can potentially help, which is good for getting reviews done

### A Concrete Example
Imagine:
- Paper #31: primary field = Machine Vision (3), secondary = Robotics (4)
- Reviewer #1: primary = Machine Vision (3), no secondary
- Reviewer #2: primary = Graphics (1), secondary = Robotics (4)

**In T2.1**: Only reviewer #1 can review paper #31 (both have vision as primary)
**In T2.4**: Both reviewers can review it! Reviewer #1 because primary matches, reviewer #2 because their secondary (robotics) matches the paper's secondary

### Ways to Make It Faster (If Needed)
- **Filter the connections**: Don't connect a reviewer to every paper they *technically* could review, only relevant ones
- **Precompute compatibility**: Build a compatibility matrix before running the algorithm
- **Cache results**: Remember which reviewer-paper pairs are feasible
- **Use approximations**: If the optimal solution takes forever, maybe use a greedy approach instead
