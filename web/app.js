"use strict";

const byId = (id) => document.getElementById(id);
const state = { account: null, pin: "" };
const currencyFormatter = new Intl.NumberFormat("en-US", {
    style: "currency",
    currency: "USD"
});

const welcomeView = byId("welcome-view");
const dashboardView = byId("dashboard-view");
const serverStatus = byId("server-status");
const toastRegion = byId("toast-region");
const deleteDialog = byId("delete-dialog");

function setServerStatus(kind, message) {
    serverStatus.className = `server-status ${kind}`;
    serverStatus.lastElementChild.textContent = message;
}

function showToast(message, kind = "success") {
    const toast = document.createElement("div");
    toast.className = `toast ${kind}`;
    toast.textContent = message;
    toastRegion.append(toast);
    window.setTimeout(() => toast.remove(), 4200);
}

function setFormBusy(form, busy, busyLabel = "Working...") {
    const button = form.querySelector('button[type="submit"]');
    if (!button) return;
    if (busy) {
        button.dataset.originalLabel = button.textContent;
        button.textContent = busyLabel;
    } else if (button.dataset.originalLabel) {
        button.textContent = button.dataset.originalLabel;
    }
    button.disabled = busy;
    Array.from(form.elements).forEach((element) => {
        if (element !== button) element.disabled = busy;
    });
}

async function apiRequest(path, options = {}) {
    let response;
    try {
        response = await fetch(path, {
            ...options,
            headers: { "Content-Type": "application/json", ...(options.headers || {}) }
        });
    } catch (_) {
        setServerStatus("disconnected", "Local server offline");
        throw new Error("The C++ server is not running. Start it in CLion and try again.");
    }

    const contentType = response.headers.get("content-type") || "";
    const result = contentType.includes("application/json")
        ? await response.json()
        : {};

    if (!response.ok) {
        throw new Error(result.error || "The request could not be completed.");
    }
    return result;
}

function formatCurrency(cents) {
    return currencyFormatter.format(Number(cents) / 100);
}

function formatAccountNumber(value) {
    return String(value).replace(/(\d{4})(?=\d)/g, "$1 ");
}

function titleCase(value) {
    const text = String(value || "");
    return text ? text.charAt(0).toUpperCase() + text.slice(1) : "";
}

function parseAmountToCents(value) {
    const normalized = value.trim().replace(/,/g, "");
    if (!/^(?:0|[1-9]\d*)(?:\.\d{1,2})?$/.test(normalized)) {
        throw new Error("Enter a valid amount with no more than two decimal places.");
    }
    const [dollars, fraction = ""] = normalized.split(".");
    const cents = (BigInt(dollars) * 100n) + BigInt(fraction.padEnd(2, "0"));
    if (cents < 1n || cents > 100000000000n) {
        throw new Error("Amount must be between $0.01 and $1,000,000,000.00.");
    }
    return Number(cents);
}

function timestampText(value) {
    if (typeof value !== "number") return "Recorded transaction";
    return new Intl.DateTimeFormat("en-US", {
        month: "short", day: "numeric", year: "numeric",
        hour: "numeric", minute: "2-digit"
    }).format(new Date(value));
}

function normalizedTransactionType(value) {
    return String(value).toLowerCase().includes("withdraw")
        ? "withdrawal"
        : "deposit";
}

function renderTransactions(transactions) {
    const list = byId("transaction-list");
    list.replaceChildren();
    const entries = Object.values(transactions || {}).sort(
        (first, second) => (second.createdAt || 0) - (first.createdAt || 0)
    );

    byId("transaction-count").textContent =
        `${entries.length} transaction${entries.length === 1 ? "" : "s"}`;

    if (entries.length === 0) {
        const empty = document.createElement("div");
        empty.className = "empty-message";
        empty.textContent = "No transactions yet. Your account activity will appear here.";
        list.append(empty);
        return;
    }

    for (const transaction of entries) {
        const type = normalizedTransactionType(transaction.type);
        const row = document.createElement("div");
        row.className = "transaction-row";

        const icon = document.createElement("span");
        icon.className = `transaction-icon ${type}`;
        icon.textContent = type === "deposit" ? "+" : "−";
        icon.setAttribute("aria-hidden", "true");

        const description = document.createElement("div");
        description.className = "transaction-description";
        const name = document.createElement("strong");
        name.textContent = titleCase(type);
        const date = document.createElement("span");
        date.textContent = timestampText(transaction.createdAt);
        description.append(name, date);

        const amount = document.createElement("div");
        amount.className = `transaction-amount ${type}`;
        amount.append(`${type === "deposit" ? "+" : "−"}${formatCurrency(transaction.amountCents)}`);
        const balance = document.createElement("span");
        balance.textContent = `${formatCurrency(transaction.balanceAfterCents)} after`;
        amount.append(balance);

        row.append(icon, description, amount);
        list.append(row);
    }
}

function renderDashboard() {
    const account = state.account;
    byId("dashboard-name").textContent = account.holderName;
    byId("account-balance").textContent = formatCurrency(account.balanceCents);
    byId("dashboard-account-number").textContent = formatAccountNumber(account.accountNumber);
    byId("account-type-badge").textContent = account.accountType;
    byId("detail-holder").textContent = account.holderName;
    byId("detail-type").textContent = account.accountType;
    renderTransactions(account.transactions);
}

function openAccount(account, pin) {
    state.account = account;
    state.pin = pin;
    renderDashboard();
    welcomeView.hidden = true;
    dashboardView.hidden = false;
    window.scrollTo({ top: 0, behavior: "smooth" });
}

function exitAccount(message = "Returned to the main menu. Your PIN was cleared from this browser.") {
    state.account = null;
    state.pin = "";
    dashboardView.hidden = true;
    welcomeView.hidden = false;
    byId("access-pin").value = "";
    byId("new-pin").value = "";
    byId("confirm-pin").value = "";
    window.scrollTo({ top: 0, behavior: "smooth" });
    showToast(message);
}

async function checkServerConnection() {
    try {
        await apiRequest("/api/health");
        setServerStatus("connected", "Local server connected");
    } catch (_) {
        setServerStatus("disconnected", "Local server offline");
    }
}

byId("create-account-form").addEventListener("submit", async (event) => {
    event.preventDefault();
    const form = event.currentTarget;
    const pin = byId("create-pin").value;
    setFormBusy(form, true, "Creating...");
    try {
        const account = await apiRequest("/api/accounts", {
            method: "POST",
            body: JSON.stringify({
                holderName: byId("holder-name").value.trim(),
                accountType: byId("account-type").value,
                pin
            })
        });
        form.reset();
        openAccount(account, pin);
        showToast(`Account created. Your number is ${formatAccountNumber(account.accountNumber)}.`);
    } catch (error) {
        showToast(error.message, "error");
    } finally {
        setFormBusy(form, false);
    }
});

byId("access-account-form").addEventListener("submit", async (event) => {
    event.preventDefault();
    const form = event.currentTarget;
    const accountNumber = byId("account-number").value.trim();
    const pin = byId("access-pin").value;
    setFormBusy(form, true, "Verifying...");
    try {
        const account = await apiRequest(`/api/accounts/${accountNumber}/access`, {
            method: "POST",
            body: JSON.stringify({ pin })
        });
        form.reset();
        openAccount(account, pin);
        showToast("Account access granted.");
    } catch (error) {
        byId("access-pin").value = "";
        showToast(error.message, "error");
    } finally {
        setFormBusy(form, false);
    }
});

byId("holder-search-form").addEventListener("submit", async (event) => {
    event.preventDefault();
    const form = event.currentTarget;
    const resultsElement = byId("search-results");
    setFormBusy(form, true, "Searching...");
    try {
        const result = await apiRequest("/api/accounts/search", {
            method: "POST",
            body: JSON.stringify({ holderName: byId("search-holder-name").value.trim() })
        });
        resultsElement.replaceChildren();
        resultsElement.hidden = false;

        if (result.accounts.length === 0) {
            const empty = document.createElement("div");
            empty.className = "empty-message";
            empty.textContent = "No accounts matched that exact holder name.";
            resultsElement.append(empty);
            return;
        }

        for (const account of result.accounts) {
            const row = document.createElement("div");
            row.className = "search-result";
            const details = document.createElement("div");
            const name = document.createElement("strong");
            name.textContent = account.holderName;
            const summary = document.createElement("span");
            summary.textContent = `${titleCase(account.accountType)} · ${formatAccountNumber(account.accountNumber)}`;
            details.append(name, summary);
            const useButton = document.createElement("button");
            useButton.type = "button";
            useButton.className = "button secondary";
            useButton.textContent = "Use account";
            useButton.addEventListener("click", () => {
                byId("account-number").value = account.accountNumber;
                byId("access-pin").focus();
                window.scrollTo({ top: byId("access-account-form").offsetTop - 120, behavior: "smooth" });
            });
            row.append(details, useButton);
            resultsElement.append(row);
        }
    } catch (error) {
        showToast(error.message, "error");
    } finally {
        setFormBusy(form, false);
    }
});

async function submitTransaction(form, type, inputId) {
    const input = byId(inputId);
    let amountCents;
    try {
        amountCents = parseAmountToCents(input.value);
    } catch (error) {
        showToast(error.message, "error");
        input.focus();
        return;
    }

    setFormBusy(form, true, type === "deposit" ? "Depositing..." : "Withdrawing...");
    try {
        state.account = await apiRequest(
            `/api/accounts/${state.account.accountNumber}/transactions`,
            {
                method: "POST",
                body: JSON.stringify({ pin: state.pin, type, amountCents })
            }
        );
        input.value = "";
        renderDashboard();
        showToast(`${titleCase(type)} of ${formatCurrency(amountCents)} completed.`);
    } catch (error) {
        showToast(error.message, "error");
    } finally {
        setFormBusy(form, false);
    }
}

byId("deposit-form").addEventListener("submit", (event) => {
    event.preventDefault();
    submitTransaction(event.currentTarget, "deposit", "deposit-amount");
});

byId("withdraw-form").addEventListener("submit", (event) => {
    event.preventDefault();
    submitTransaction(event.currentTarget, "withdrawal", "withdraw-amount");
});

byId("change-pin-form").addEventListener("submit", async (event) => {
    event.preventDefault();
    const form = event.currentTarget;
    const newPin = byId("new-pin").value;
    if (newPin !== byId("confirm-pin").value) {
        showToast("The new PIN entries do not match.", "error");
        return;
    }

    setFormBusy(form, true, "Updating...");
    try {
        state.account = await apiRequest(`/api/accounts/${state.account.accountNumber}/pin`, {
            method: "POST",
            body: JSON.stringify({ pin: state.pin, newPin })
        });
        state.pin = newPin;
        form.reset();
        renderDashboard();
        showToast("PIN updated successfully.");
    } catch (error) {
        showToast(error.message, "error");
    } finally {
        setFormBusy(form, false);
    }
});

byId("exit-account-button").addEventListener("click", () => exitAccount());
byId("open-delete-dialog").addEventListener("click", () => {
    byId("delete-confirmation").value = "";
    deleteDialog.showModal();
});
byId("cancel-delete").addEventListener("click", () => deleteDialog.close());

byId("delete-account-form").addEventListener("submit", async (event) => {
    event.preventDefault();
    const form = event.currentTarget;
    if (byId("delete-confirmation").value !== "DELETE") {
        showToast("Type DELETE exactly to confirm.", "error");
        return;
    }

    setFormBusy(form, true, "Deleting...");
    try {
        await apiRequest(`/api/accounts/${state.account.accountNumber}`, {
            method: "DELETE",
            body: JSON.stringify({ pin: state.pin })
        });
        deleteDialog.close();
        exitAccount("Account deleted permanently.");
    } catch (error) {
        showToast(error.message, "error");
    } finally {
        setFormBusy(form, false);
    }
});

checkServerConnection();
