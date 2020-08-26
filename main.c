#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define SIZE 6

typedef struct tree {
    char names[SIZE];
    struct tree* left;
    struct tree* right;
}tree;

tree* insert(tree *root,tree *element);
tree* createBST(tree* root);
tree* createNode(char* name);
void inorder(tree *root, FILE* ouput);
void preorder(tree *root, FILE* ouput);
void postorder(tree *root, FILE* ouput);
int totalCharacters(tree* root);
int isBalanced(tree* root);
int max(int a, int b);
int height(tree* root);
int countBefore(tree* root, tree* trueRoot, char name[]);
int find(tree* p, char name[]);
tree* deleteName(tree* root, char name[]);
tree* parent(tree *root, struct tree *node);
tree* findNode(tree *p, char name[]);
int hasOnlyRightChild(struct tree *node);
int hasOnlyLeftChild(struct tree *node);
int isLeaf(struct tree *node);
tree* maxVal(tree *root);
tree* minVal(tree *root);

int main(void) {

    tree* root = NULL;
    FILE* output = fopen("out.txt", "w");
    root = createBST(root);
		tree* trueRoot = root;

    fprintf(output, "Pre Order: ");
    preorder(root, output);
    fprintf(output, "\nIn Order: ");
    inorder(root, output);
    fprintf(output, "\nPost Order: ");
    postorder(root, output);

    fprintf(output, "\nTotal characters: %d", totalCharacters(root));
    fprintf(output, "\nHeight left = %d", height(root->left)-1);
    fprintf(output, ", Height right = %d", height(root->right)-1);

    switch(isBalanced(root)) {
        case 1: fprintf(output, ". The tree is balanced."); break;
        case 0: fprintf(output, ". The tree is imbalanced."); break;
    }

    FILE* input = fopen("in.txt", "r");

    int n,s,d;
    char ignore[1024];
    char name[SIZE];

    fscanf(input, "%d %d %d", &n, &s, &d);

    // Skip to deleted names
    for(int i=0; i<n+1; i++)
        fgets(ignore, sizeof(ignore), input);

    // Search Phase
    fprintf(output, "\nSearch Phase:");
    for(int i=0; i<s; i++) {
        fscanf(input, "%s", name);
        fprintf(output, "\n%s: ", name);

        if(find(root, name) == 1) fprintf(output, "Found, ");
        else fprintf(output, "Not found, ");

        // WIP
        int n;
        if(find(root, name) == 0)
            n = 0;
        else
            n = countBefore(root, trueRoot, name);

        fprintf(output, "Items before: %d", n);
    }

    // Delete phase
    fprintf(output, "\nDelete Phase: ");
    for(int i=0; i<d; i++) {
        fscanf(input, "%s", name);
        root = deleteName(root, name);
        fprintf(output, "\n%s: deleted", name);
    }

    // Reprint
    fprintf(output, "\nIn Order: ");
    inorder(root, output);
    fprintf(output, "\nTotal characters: %d", totalCharacters(root));

    fclose(input);
    fclose(output);
    return 0;
}

int countBefore(tree* root, tree* trueRoot, char name[]) {

    // if root NULL
    if(root==NULL || (strcmp(name, root->names) == 0 && root->left == NULL)) 
	return 0;

    // input < root
    if(strcmp(name, root->names) < 0)
        return countBefore(root->left, trueRoot, name);

    // input > root
    if(strcmp(name, root->names) > 0) 
        return 1 + countBefore(root->left, trueRoot, name) + countBefore(root->right, trueRoot, name);

    // if input == the actual root
    if(strcmp(name, trueRoot->names) == 0)
        return countBefore(root->left, trueRoot, name);

    // if input == root AND root left is not NULL
    if(strcmp(name, root->names) == 0 && root->left != NULL) 
        return 1 + countBefore(root->left, trueRoot, name);
    
    return 0;
}

tree* parent(tree *root, tree *node) {

    // Take care of NULL cases.
    if (root == NULL || root == node)
        return NULL;

    // The root is the direct parent of node.
    if (root->left == node || root->right == node)
        return root;

    // Look for node's parent in the left side of the tree.
    if (strcmp(node->names, root->names) < 0)
        return parent(root->left, node);

    // Look for node's parent in the right side of the tree.
    else if (strcmp(node->names, root->names) > 0)
        return parent(root->right, node);

    return NULL; // Catch any other extraneous cases.

}

// Returns a pointer to the node storing the minimum value in the tree
// with the root, root. Will not work if called with an empty tree.
tree* minVal(tree *root) {

    // Root stores the minimal value.
    if (root->left == NULL)
        return root;

        // The left subtree of the root stores the minimal value.
    else
        return minVal(root->left);
}


// Returns a pointer to the node storing the maximum value in the tree
// with the root, root. Will not work if called with an empty tree.
tree* maxVal(tree *root) {

    // Root stores the maximal value.
    if (root->right == NULL)
        return root;

        // The right subtree of the root stores the maximal value.
    else
        return maxVal(root->right);
}

// Returns 1 if node is a leaf node, 0 otherwise.
int isLeaf(struct tree *node) {

    return (node->left == NULL && node->right == NULL);
}

// Returns 1 iff node has a left child and no right child.
int hasOnlyLeftChild(struct tree *node) {
    return (node->left!= NULL && node->right == NULL);
}

// Returns 1 iff node has a right child and no left child.
int hasOnlyRightChild(struct tree *node) {
    return (node->left== NULL && node->right != NULL);
}

tree* findNode(tree *p, char name[]) {

    // Check if there are nodes in the tree.
    if (p != NULL) {

        // Found the value at the root.

        if (strcmp(p->names, name) == 0)
            return p;

        // Search to the left.
        if (strcmp(p->names, name) > 0)
            return findNode(p->left, name);

            // Or...search to the right.
        else
            return findNode(p->right, name);

    }
    else
        return NULL; // No node found.
}

tree* deleteName(tree* root, char name[]) {

    tree *delnode, *new_del_node, *save_node;
    tree *par;
    char save_name[SIZE];

    delnode = findNode(root, name); // Get a pointer to the node to delete.

    par = parent(root, delnode); // Get the parent of this node.

    // Take care of the case where the node to delete is a leaf node.
    if (isLeaf(delnode)) {

        // Deleting the only node in the tree.
        if (par == NULL) {
            free(root); // free the memory for the node.
            return NULL;
        }

        // Deletes the node if it's a left child.

        if (strcmp(name, par->names) < 0) {
            free(par->left); // Free the memory for the node.
            par->left = NULL;
        }

            // Deletes the node if it's a right child.
        else {
            free(par->right); // Free the memory for the node.
            par->right = NULL;
        }

        return root; // Return the root of the new tree.
    }

    // Take care of the case where the node to be deleted only has a left
    // child.
    if (hasOnlyLeftChild(delnode)) {

        // Deleting the root node of the tree.
        if (par == NULL) {
            save_node = delnode->left;
            free(delnode); // Free the node to delete.
            return save_node; // Return the new root node of the resulting tree.
        }

        // Deletes the node if it's a left child.
        if (strcmp(name, par->names) < 0) {
            save_node = par->left; // Save the node to delete.
            par->left = par->left->left; // Readjust the parent pointer.
            free(save_node); // Free the memory for the deleted node.
        }

            // Deletes the node if it's a right child.
        else {
            save_node = par->right; // Save the node to delete.
            par->right = par->right->left; // Readjust the parent pointer.
            free(save_node); // Free the memory for the deleted node.
        }

        return root; // Return the root of the tree after the deletion.
    }

    // Takes care of the case where the deleted node only has a right child.
    if (hasOnlyRightChild(delnode)) {

        // Node to delete is the root node.
        if (par == NULL) {
            save_node = delnode->right;
            free(delnode);
            return save_node;
        }

        // Delete's the node if it is a left child.
        if (strcmp(name, par->names) < 0) {
            save_node = par->left;
            par->left = par->left->right;
            free(save_node);
        }

            // Delete's the node if it is a right child.
        else {
            save_node = par->right;
            par->right = par->right->right;
            free(save_node);
        }
        return root;
    }

    // Find the new physical node to delete.
    new_del_node = minVal(delnode->right);
    strcpy(save_name, new_del_node->names);

    deleteName(root, save_name);  // Now, delete the proper value.

    // Restore the data to the original node to be deleted.
    strcpy(delnode->names, save_name);

    return root;
}

int find(tree* p, char name[]) {

    // Check if there are nodes in the tree.
    if (p != NULL) {

        // Found the value at the root.
        if (strcmp(p->names, name) == 0)
            return 1;

        // Search to the left.
        if (strcmp(name, p->names) < 0)
            return find(p->left, name);

            // Or...search to the right.
        else
            return find(p->right, name);

    }
    else
        return 0;
}


int isBalanced(tree* root) {
    int rh, lh;

    if(root==NULL) return 1;

    lh = height(root->left);
    rh = height(root->right);

    if (abs(lh - rh) <= 1 && isBalanced(root->left) && isBalanced(root->right))
        return 1;

    return 0;
}

int max(int a, int b)  {
    return (a >= b) ? a : b;
}


int height(tree* root) {
    /* base case tree is empty */
    if (root == NULL)
        return 0;

    /* If tree is not empty then height = 1 + max of left
      height and right heights */
    return 1 + max(height(root->left), height(root->right));
}

int totalCharacters(tree* root) {
    if (root != NULL)
        return strlen(root->names) + totalCharacters(root->left) + totalCharacters(root->right);
    else
        return 0;
}

void inorder(tree *p, FILE* output) {
    if(p!=NULL) {
        inorder(p->left, output);
        fprintf(output ,"%s ", p->names);
        inorder(p->right, output);
    }

}

void preorder(tree *p, FILE* output) {
    if(p!=NULL) {
        fprintf(output ,"%s ", p->names);
        preorder(p->left, output);
        preorder(p->right, output);
    }

}

void postorder(tree *p, FILE* output) {
    if(p!=NULL) {
        postorder(p->left, output);
        postorder(p->right, output);
        fprintf(output ,"%s ", p->names);
    }

}

tree* createNode(char* name) {
    // Allocate space for the node, set the fields.
    tree* temp = malloc(sizeof(tree));
    strcpy(temp->names, name);
    temp->left = NULL;
    temp->right = NULL;

    return temp; // Return a pointer to the created node.
}

tree* createBST(tree* root) {
    int n,s,d;
    char name[SIZE];
    tree* temp;
    FILE* input = fopen("in.txt", "r");

    fscanf(input,"%d %d %d", &n, &s, &d);

    for(int i = 0; i < n; i++){
        fscanf(input, "%s", name);
        temp = createNode(name);
        root = insert(root, temp);
    }

    fclose(input);
    return root;
}

tree* insert(tree *root,tree *element) {

    // Inserting into an empty tree.
    if (root == NULL)
        return element;

    else {

        // element should be inserted to the right.
        if (strcmp(element->names, root->names) > 0) {

            // There is a right subtree to insert the node.
            if (root->right != NULL)
                root->right = insert(root->right, element);

                // Place the node directly to the right of root.
            else
                root->right = element;
        }

            // element should be inserted to the left.
        else {

            // There is a left subtree to insert the node.
            if (root->left != NULL)
                root->left = insert(root->left, element);

                // Place the node directly to the left of root.
            else
                root->left = element;
        }

        // Return the root pointer of the updated tree.
        return root;
    }
}
