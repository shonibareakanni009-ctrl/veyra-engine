const sources = ['data/aether.json', 'data/forge.json'];
const storageKey = 'veyra-lead-dev-messages';
const list = document.querySelector('#messageList');
const form = document.querySelector('#messageForm');
const textInput = document.querySelector('#messageText');
const typeInput = document.querySelector('#messageType');
const charCount = document.querySelector('#charCount');
const clearButton = document.querySelector('#clearMessages');

const escapeHtml = (value) => value.replace(/[&<>'"]/g, (character) => ({'&':'&amp;','<':'&lt;','>':'&gt;',"'":'&#39;','"':'&quot;'}[character]));
const formatTime = (timestamp) => new Intl.DateTimeFormat(undefined, {dateStyle:'medium', timeStyle:'short'}).format(new Date(timestamp));
const readMine = () => JSON.parse(localStorage.getItem(storageKey) || '[]');
const writeMine = (messages) => localStorage.setItem(storageKey, JSON.stringify(messages));

function render(messages) {
  if (!messages.length) { list.innerHTML = '<p class="loading">No messages yet. Start the conversation.</p>'; return; }
  list.innerHTML = messages.sort((a,b) => new Date(a.timestamp) - new Date(b.timestamp)).map((message) => {
    const className = message.agent === 'Lead Dev' ? 'lead' : message.agent === 'Forge' ? 'developer' : 'manager';
    const initial = message.agent === 'Lead Dev' ? 'Y' : message.agent[0];
    return `<article class="message ${className}"><div class="avatar" aria-hidden="true">${escapeHtml(initial)}</div><div class="bubble"><div class="message-meta"><strong>${escapeHtml(message.agent)}</strong>${escapeHtml(message.type)} · ${escapeHtml(formatTime(message.timestamp))}</div><p class="message-text">${escapeHtml(message.text)}</p></div></article>`;
  }).join('');
}

async function load() {
  try {
    const logs = await Promise.all(sources.map((source) => fetch(source).then((response) => { if (!response.ok) throw new Error(`Could not load ${source}`); return response.json(); })));
    const repositoryMessages = logs.flatMap((log) => log.messages.map((message) => ({...message, agent: log.agent, role: log.role})));
    render(repositoryMessages.concat(readMine()));
  } catch (error) { list.innerHTML = `<p class="loading">Communication logs could not be loaded. ${escapeHtml(error.message)}</p>`; }
}

textInput.addEventListener('input', () => { charCount.textContent = `${textInput.value.length} / 1000`; });
form.addEventListener('submit', (event) => { event.preventDefault(); const text = textInput.value.trim(); if (!text) return; const mine = readMine(); mine.push({id:`lead-${Date.now()}`, agent:'Lead Dev', type:typeInput.value, timestamp:new Date().toISOString(), text}); writeMine(mine); textInput.value = ''; charCount.textContent = '0 / 1000'; load(); });
clearButton.addEventListener('click', () => { localStorage.removeItem(storageKey); load(); });
load();
