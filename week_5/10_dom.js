/* Week 5: DOM Manipulation - Filter, Add Article */

// Show filter menu, hide add form
function showFilter() {
    document.getElementById("filterContent").style.display = "block";
    document.getElementById("newContent").style.display = "none";
}

// Show add form, hide filter menu
function showAddNew() {
    document.getElementById("newContent").style.display = "flex";
    document.getElementById("filterContent").style.display = "none";
}

// Filter articles by checkbox state
function filterArticles() {
    const opinionChecked = document.getElementById("opinionCheckbox").checked;
    const recipeChecked = document.getElementById("recipeCheckbox").checked;
    const updateChecked = document.getElementById("updateCheckbox").checked;

    document.querySelectorAll("#articleList article.opinion").forEach(function (el) {
        el.style.display = opinionChecked ? "" : "none";
    });
    document.querySelectorAll("#articleList article.recipe").forEach(function (el) {
        el.style.display = recipeChecked ? "" : "none";
    });
    document.querySelectorAll("#articleList article.update").forEach(function (el) {
        el.style.display = updateChecked ? "" : "none";
    });
}

// Add new article to the list
function addNewArticle() {
    const title = document.getElementById("inputHeader").value.trim();
    const text = document.getElementById("inputArticle").value.trim();

    if (!title || !text) return;

    let type, typeLabel;
    if (document.getElementById("opinionRadio").checked) {
        type = "opinion";
        typeLabel = "Opinion";
    } else if (document.getElementById("recipeRadio").checked) {
        type = "recipe";
        typeLabel = "Recipe";
    } else if (document.getElementById("lifeRadio").checked) {
        type = "update";
        typeLabel = "Update";
    } else {
        return;
    }

    const article = document.createElement("article");
    article.className = type;
    article.id = "a" + (document.querySelectorAll("#articleList article").length + 1);

    const marker = document.createElement("span");
    marker.className = "marker";
    marker.textContent = typeLabel;

    const h2 = document.createElement("h2");
    h2.textContent = title;

    const p = document.createElement("p");
    p.textContent = text;

    article.appendChild(marker);
    article.appendChild(h2);
    article.appendChild(p);

    if (text.length > 100) {
        const readMore = document.createElement("p");
        const link = document.createElement("a");
        link.href = "moreDetails.html";
        link.textContent = "Read more...";
        readMore.appendChild(link);
        article.appendChild(readMore);
    }

    document.getElementById("articleList").appendChild(article);

    document.getElementById("inputHeader").value = "";
    document.getElementById("inputArticle").value = "";

    filterArticles();
}
