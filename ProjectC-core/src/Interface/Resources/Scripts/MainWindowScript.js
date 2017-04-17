var showModule = function (name, isCoherent) {
    var nav = document.getElementById("innerNavigation");
    var items = nav.getElementsByTagName("button");
    if (items.length > 0) {
        var item = items[items.length - 1];
        item.classList.add("navButton");
        item.classList.remove("navButtonDisabled");
    }

    var container = document.createElement("div");
    container.style.opacity = "0";

    if (isCoherent && nav.hasChildNodes()) {
        var arrow = document.createElement("div");
        arrow.classList.add("arrow");
        container.appendChild(arrow);
    }

    var button = document.createElement("button");
    button.classList.add("navButtonDisabled");
    button.textContent = name;
    container.appendChild(button);

    nav.appendChild(container);
    container.classList.add("navFadeInAnimation");
}

var popModule = function () {
    var nav = document.getElementById("innerNavigation");
    if (nav.children.length > 0) {
        nav.removeChild(nav.lastChild);
    }
    if (nav.lastChild) {
        nav.lastChild.classList.remove("navButton");
        nav.lastChild.classList.add("navButtonDisabled");
    }
}