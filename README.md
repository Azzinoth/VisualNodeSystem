# Visual Node System - Media Branch

This branch serves as a dedicated storage for media assets used in the README.md file of the master branch.

If you want to create a similar setup in your repository, follow these quick steps, which are also outlined in this [original tutorial](https://medium.com/@minamimunakata/how-to-store-images-for-use-in-readme-md-on-github-9fb54256e951):

```bash
# Create a new orphan branch called 'media'
git checkout --orphan media

# Remove all files from the new branch
git rm -rf .
```

Next, copy the images you need into the folder where your 'media' branch resides.

```bash
# Add all new files to the branch
git add --all

# Commit the changes
git commit -m "First commit."

# Push changes. Origin should be path to your repository.
git push origin media

# Switch back to the master branch
git checkout master
```

Remember, this branch should only be used for storing images and other media files required for the documentation, and not for code or other project resources.
