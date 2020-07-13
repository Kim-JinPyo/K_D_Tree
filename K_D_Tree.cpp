#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_DIM 3

struct kd_node_t {
	double x[MAX_DIM];
	struct kd_node_t *left, *right;
};

void point_search(struct kd_node_t * root, int x, int y, int *c);
void range_search(struct kd_node_t * root, int x, int y, int width, int height, int *c);
void nearest_neighbor_search(struct kd_node_t * root, struct kd_node_t test_node, struct kd_node_t storing_node[], int *index, double *dist);

inline void swap(struct kd_node_t *x, struct kd_node_t *y) {
	double tmp[MAX_DIM];
	memcpy(tmp, x->x, sizeof(tmp));
	memcpy(x->x, y->x, sizeof(tmp));
	memcpy(y->x, tmp, sizeof(tmp));
}

/* The find_median function uses the quickselect algorithm, which is similar to the quicksort sorting algorithm , to select the median from a large number of points.*/

struct kd_node_t* find_median(struct kd_node_t *start, struct kd_node_t *end, int idx) {
	if (end <= start) return NULL;
	if (end == start + 1)         return start;
	struct kd_node_t *p, *store, *md = start + (end - start) / 2;
	double pivot;
	while (1) {
		pivot = md->x[idx];
		swap(md, end - 1);
		for (store = p = start; p < end; p++) {
			if (p->x[idx] < pivot) {
				if (p != store)                     swap(p, store);
				store++;
			}
		}
		swap(store, end - 1);
		/* median has duplicate values */
		if (store->x[idx] == md->x[idx])             return md;
		if (store > md) end = store; else        start = store;
	}
}

struct kd_node_t* make_tree(struct kd_node_t *t, int len, int i, int dim) {
	struct kd_node_t *n;
	if (!len) return 0;
	if ((n = find_median(t, t + len, i))) {
		i = (i + 1) % dim;
		n->left = make_tree(t, n - t, i, dim);
		n->right = make_tree(n + 1, t + len - (n + 1), i, dim);
	}
	return n;
}

int main(void) {

	struct kd_node_t wp[] = { 
		{{2, 3}}, {{5, 4}}, {{3, 4}}, {{9, 6}}, {{4, 7}}, {{8, 1}}, {{7, 2}}
	};

	struct kd_node_t testNode = {{9, 2}};
	struct kd_node_t *root;
	root = make_tree(wp, sizeof(wp) / sizeof(wp[1]), 0, 2);

	int count = 0; //Variable to determine whether there is a corresponding point or not
	point_search(root, 5, 4, &count);
	if (count == 0)
		printf("There's no point(5, 4)\n");
	count = 0;

	point_search(root, 4, 7, &count);
	if (count == 0)
		printf("There's no point(4, 7)\n");
	count = 0;

	point_search(root, 10, 5, &count);
	if (count == 0)
		printf("There's no point(10, 5)\n");
	count = 0;

	range_search(root, 6, 3, 3, 4, &count);
	if (count == 0)
		printf("There's no point in range\n");
	count = 0;
	
	struct kd_node_t test1 = { {5, 4} }, test2 = { {4, 7} };

	struct kd_node_t storing_node[10]; //Array to store all nodes with the nearest point
	int indexOfStore; //storing_node's index
	double distance; //Distance between test node's point and node's point in tree. finally, the distance from the node with the nearest point will be stored.
	indexOfStore = 0;
	distance = (root->x[0] - test1.x[0]) * (root->x[0] - test1.x[0]) + (root->x[1] - test1.x[1]) * (root->x[1] - test1.x[1]);
	storing_node[0].x[0] = root->x[0];
	storing_node[0].x[1] = root->x[1];
	nearest_neighbor_search(root, test1, storing_node, &indexOfStore, &distance);

	printf("nearest point : ");
	for (int i = 0; i < indexOfStore; i++)
		printf("(%.0f, %.0f) ", storing_node[i].x[0], storing_node[i].x[1]);
	printf("\n");

	indexOfStore = 0;
	distance = (root->x[0] - test2.x[0]) * (root->x[0] - test2.x[0]) + (root->x[1] - test2.x[1]) * (root->x[1] - test2.x[1]);
	storing_node[0].x[0] = root->x[0];
	storing_node[0].x[1] = root->x[1];
	nearest_neighbor_search(root, test2, storing_node, &indexOfStore, &distance);

	printf("nearest point : ");
	for (int i = 0; i < indexOfStore; i++)
		printf("(%.0f, %.0f) ", storing_node[i].x[0], storing_node[i].x[1]);
	printf("\n");
	
	return 0;
}

void point_search(struct kd_node_t * root, int x, int y, int *c)
{
	if (root != NULL)
	{
		//similar to inorder traversal
		point_search(root->left, x, y, c);

		if (root->x[0] == x && root->x[1] == y)
		{
			printf("Find point : (%.0f, %.0f)\n", root->x[0], root->x[1]);
			(*c)++; //if there's no node with the corresponding point, *c ,which points variable count, will 0.
		}

		point_search(root->right, x, y, c);
	}
}

void range_search(struct kd_node_t * root, int x, int y, int width, int height, int *c)
{
	if (root != NULL)
	{
		//similar to point search function
		range_search(root->left, x, y, width, height, c);

		if (root->x[0] > x && root->x[0] < x + width && root->x[1] > y && root->x[1] < y + height) //Borders are not included
		{
			printf("Find point in range (%d, %d) ~ (%d, %d) : (%.0f, %.0f)\n", x, y, (x + width), (y + height), root->x[0], root->x[1]);
			(*c)++;
		}

		range_search(root->right, x, y, width, height, c);
	}
}

void nearest_neighbor_search(struct kd_node_t * root, struct kd_node_t test_node, struct kd_node_t storing_node[], int *index, double *dist)
{
	if (root != NULL)
	{
		//similar to inorder traversal
		nearest_neighbor_search(root->left, test_node, storing_node, index, dist);

		double d = (root->x[0] - test_node.x[0]) * (root->x[0] - test_node.x[0]) + (root->x[1] - test_node.x[1]) * (root->x[1] - test_node.x[1]);

		if (*dist == d) //if closest node is more exist
		{
			storing_node[(*index)].x[0] = root->x[0];
			storing_node[(*index)].x[1] = root->x[1];
			(*index)++;
		}

		else if (*dist > d) //if there is a node closer
		{
			(*dist) = d;
			(*index) = 0; //Initialize the index of the array that stores the node with closest point.
			storing_node[(*index)].x[0] = root->x[0];
			storing_node[(*index)].x[1] = root->x[1];
			(*index)++;
		}

		nearest_neighbor_search(root->right, test_node, storing_node, index, dist);
	}
}