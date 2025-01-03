```markdown
# Job Search App - Setup Instructions

Follow these steps to clone the project, configure the environment, and run the project.

## Step 1: Clone the Project

First, clone the repository using the following command:

git clone https://github.com/Emelloul98/job-search-app.git


## Step 2: Open the Solution

After cloning the project, open the solution in Visual Studio:

Navigate to the project directory and open the `CppApp.sln` file.
   
## Step 3: Set the Startup Project

In Visual Studio, right-click on `connectedApp` (or your main project) in the Solution Explorer and select **Set as Startup Project**.

## Step 4: Set the Environment Variable

1. Right-click on the solution in the Solution Explorer and select **Properties**.
2. In the properties window, navigate to **Configuration Properties** > **Debugging**.
3. In the **Environment** field, click the dropdown (`...`) and add the following environment variable:

   PATH=%PATH%;<ADD YOUR PATH TO THE PROJECT>\job-search-app\shared\HttpSrc\openssl\bin
4. Click **OK** to save the environment variable.


## Step 5: Build the Solution

Build the solution to ensure everything is compiled correctly:

1. Go to the **Build** menu in Visual Studio and select **Build Solution** (or press `Ctrl+Shift+B`).

## Step 6: Run the Application

To run the application, go to the **Debug** menu and select **Start Debugging** (or press `F5`).

Your application should now be up and running.
```

### Summary:
- **Clone the repository** with `git clone`.
- **Open the `.sln` file** in Visual Studio.
- **Set `connectedApp` as the startup project**.
- **Configure the environment variable** for OpenSSL in the **Properties** section.
- **Build the solution** using the **Build Solution** option.
- **Run the project** using **Start Debugging**.

