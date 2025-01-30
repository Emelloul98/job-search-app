# Job Search Application

## Overview
This project is a job search application built in C++ with a user interface developed using the **imGui** library. The application utilizes the **Adzuna API** to fetch job listings and provides various functionalities to enhance the job search experience. Below, you'll find detailed descriptions of each feature along with placeholders for corresponding images.

---

## Technologies Used
- **C++**: Core language for application logic.
- **imGui**: Library for building the user interface.
- **Adzuna API**: Source for fetching job data.
- **imPlot**: Library for drawing histograms and pie charts.

---

## Installation
1. Clone this repository.
2. Click on CppApp.sln
3. Press F5 to run the application.
---

### 1. Home Page
The home page includes a **search bar** that allows users to search for jobs based on:
- **Country**
- **Job field**
- **Full-time or part-time positions**
- **Maximum time since the job was posted**
  
![Home Page](https://github.com/Emelloul98/job-search-app/blob/main/images/Readme-images/home-page.png)

---
### 2. Table Page
After clicking on search icon, a table of the 10 most recent jobs will appear.
Additionally, users can click on the **Add More** button to fetch additional 10 job listings from the API.

![Home Page](https://github.com/Emelloul98/job-search-app/blob/table-component/images/Readme-images/add-more-button.png)

---

### 3. Job Details Popup
By clicking the **eye icon** at the end of each job row, users can open a **popup page** displaying more details about the selected job.

![Job Details Popup](https://github.com/Emelloul98/job-search-app/blob/table-component/images/Readme-images/job-details-page.png)

Users can also add jobs to their favorites and remove jobs from their favorites directly from the favorites list:

![Remove from Favorites](https://github.com/Emelloul98/job-search-app/blob/table-component/images/Readme-images/delete-from-favorites.png)

---

### 4. Favorite Jobs
Users can view all their favorite jobs in one place and save them to a text file for later reference.

![Favorites Page](https://github.com/Emelloul98/job-search-app/blob/table-component/images/Readme-images/favorite.png)

---

### 5. Highlighted Favorite Jobs
Jobs marked as favorites are **visually indicated** in the job table by a **yellow star icon** next to each row.

![Highlighted Favorites](https://github.com/Emelloul98/job-search-app/blob/table-component/images/Readme-images/favorites-star.png)

---

### 6. Salary Statistics
The app provides **statistics on average salaries** in the selected country and job field for the year 2024.

![Salary Statistics](https://github.com/Emelloul98/job-search-app/blob/table-component/images/Readme-images/statistics.png)

---

### 7. Top Companies
Users can view information about the **top 5 companies** with the most job listings in the selected country.

![Top Companies](https://github.com/Emelloul98/job-search-app/blob/table-component/images/Readme-images/pie-chart.png)

---





