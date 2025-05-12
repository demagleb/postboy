// State to node color mapping
const stateColors = {
    'NotInitialized': '#9E9E9E',   // Серый
    'Initialized': '#90CAF9',      // Светло-синий
    'Running': '#4CAF50',          // Зеленый
    'StopRequested': '#FFA726',    // Оранжевый
    'Stopped': '#F44336',          // Красный
    'ResumeRequested': '#8BC34A',  // Светло-зеленый
    'FinishRequested': '#7986CB',  // Индиго
    'Finished': '#3F51B5',         // Синий
    'SampleRequested': '#FF9800',  // Оранжевый
    'Sampling': '#9C27B0',         // Фиолетовый
    'Unknown': '#9E9E9E'           // Серый
};

const bindingColor = '#FFD700';  // Золотой

const POSTBOY_SIZE = 12;
const BINDING_WIDTH = 15;
const BINDING_HEIGHT = 10;

let graphData = {
    nodes: [],
    links: []
};

const Graph = ForceGraph()

let nodeStates = {};
let selectedNode = null;
let graphInstance = null;

async function fetchConfig() {
    try {
        const response = await fetch('/api/config');
        const data = await response.json();
        processConfig(data);
    } catch (error) {
        console.error('Error fetching configuration:', error);
    }
}

function processConfig(config) {
    graphData.nodes = [];
    graphData.links = [];

    if (config.postboy) {
        config.postboy.forEach(pb => {
            graphData.nodes.push({
                id: pb.name,
                name: pb.name,
                process: pb.process,
                type: 'postboy',
                config: pb
            });

            fetchNodeState(pb.name);
        });
    }

    if (config.binding) {
        config.binding.forEach(binding => {
            const bindingId = `binding_${binding.name}`;
            graphData.nodes.push({
                id: bindingId,
                name: binding.name,
                type: 'binding',
                config: binding
            });

            binding.producer.forEach(producer => {
                graphData.links.push({
                    source: producer.postboyName,
                    target: bindingId,
                    name: `produce_${binding.name}`
                });
            });

            binding.consumer.forEach(consumer => {
                graphData.links.push({
                    source: bindingId,
                    target: consumer.postboyName,
                    name: `consume_${binding.name}`
                });
            });
        });
    }

    updateGraph();
}

async function fetchNodeState(nodeName) {
    try {
        const response = await fetch(`/api/postboy/${nodeName}/state`);
        const data = await response.json();
        nodeStates[nodeName] = data.state;

        await fetchNodeLatencies(nodeName);

        updateGraph()

        if (selectedNode && selectedNode.id === nodeName) {
            updateSidebar(selectedNode);
        }
    } catch (error) {
        console.error(`Error fetching state for node ${nodeName}:`, error);
        nodeStates[nodeName] = 'Unknown';
    }
}

async function fetchNodeConfig(nodeName) {
    try {
        const response = await fetch(`/api/postboy/${nodeName}/config`);
        return await response.json();
    } catch (error) {
        console.error(`Error fetching config for node ${nodeName}:`, error);
        return null;
    }
}

async function fetchNodeLatencies(nodeName) {
    try {
        const response = await fetch(`/api/postboy/${nodeName}/latencies`);
        const data = await response.json();
        if (selectedNode && selectedNode.id === nodeName) {
            selectedNode.latencies = data;
            updateSidebar(selectedNode);
        }
    } catch (error) {
        console.error(`Error fetching latencies for node ${nodeName}:`, error);
    }
}

function updateGraph() {
    if (!graphInstance) {
        graphInstance = Graph(document.getElementById('graph-container'))
            .nodeId('id')
            .nodeLabel('name')
            .nodeRelSize(6)
            .linkLabel('name')
            .linkDirectionalArrowLength(3)
            .linkDirectionalArrowRelPos(1)
            .onNodeClick(handleNodeClick)
            .nodeCanvasObject((node, ctx, globalScale) => {
                const label = node.name;
                const fontSize = 12/globalScale;
                ctx.font = `${fontSize}px Sans-Serif`;

                let color;
                if (node.type === 'binding') {
                    color = bindingColor;
                } else {
                    color = nodeStates[node.id] ? stateColors[nodeStates[node.id]] : stateColors['Unknown'];
                }

                if (node.type === 'binding') {
                    const rectWidth = BINDING_WIDTH;
                    const rectHeight = BINDING_HEIGHT;;

                    ctx.fillStyle = color;
                    ctx.fillRect(node.x - rectWidth/2, node.y - rectHeight/2, rectWidth, rectHeight);

                    ctx.strokeStyle = '#000000';
                    ctx.lineWidth = 0.5/globalScale;
                    ctx.strokeRect(node.x - rectWidth/2, node.y - rectHeight/2, rectWidth, rectHeight);

                    ctx.fillStyle = 'black';
                    ctx.textAlign = 'center';
                    ctx.textBaseline = 'middle';
                    ctx.fillText(label, node.x, node.y);

                    node.__bWidth = rectWidth;
                    node.__bHeight = rectHeight;
                } else {
                    const nodeSize = POSTBOY_SIZE;
                    ctx.beginPath();
                    ctx.arc(node.x, node.y, nodeSize, 0, 2 * Math.PI);
                    ctx.fillStyle = color;
                    ctx.fill();
                    ctx.strokeStyle = '#000000';
                    ctx.lineWidth = 0.5/globalScale;
                    ctx.stroke();

                    ctx.fillStyle = 'black';
                    ctx.textAlign = 'center';
                    ctx.textBaseline = 'bottom';
                    ctx.fillText(label, node.x, node.y - 7);
                }
            })
            .nodePointerAreaPaint((node, color, ctx) => {
                if (node.type === 'binding') {
                    const rectWidth = node.__bWidth || 30;
                    const rectHeight = node.__bHeight || 20;
                    ctx.fillStyle = color;
                    ctx.fillRect(node.x - rectWidth/2, node.y - rectHeight/2, rectWidth, rectHeight);
                } else {
                    const nodeSize = 6;
                    ctx.beginPath();
                    ctx.arc(node.x, node.y, nodeSize, 0, 2 * Math.PI);
                    ctx.fillStyle = color;
                    ctx.fill();
                }
            });
    }
    graphInstance.graphData(graphData);
}

async function handleNodeClick(node) {
    selectedNode = node;
    if (node.type === 'postboy') {
        await fetchNodeState(node.id);

        const nodeConfig = await fetchNodeConfig(node.id);
        if (nodeConfig) {
            selectedNode.detailedConfig = nodeConfig;
        }
    }

    updateSidebar(node);
    document.getElementById('sidebar').classList.add('open');
}

function setupButtonHandlers(nodeId) {
    document.getElementById('btn-run').onclick = () => requestStateChange(nodeId, 'Run');
    document.getElementById('btn-stop').onclick = () => requestStateChange(nodeId, 'Stop');
    document.getElementById('btn-resume').onclick = () => requestStateChange(nodeId, 'Resume');
    document.getElementById('btn-finish').onclick = () => requestStateChange(nodeId, 'Finish');
    document.getElementById('btn-sample').onclick = () => requestStateChange(nodeId, 'Sample');
}

async function requestStateChange(nodeId, action) {
    try {
        const response = await fetch(`/api/postboy/${nodeId}/request${action}`, {
            method: 'POST',
            body: ""
        });

        if (response.ok) {
            setTimeout(() => {
                fetchNodeState(nodeId);
            }, 500);
        }
    } catch (error) {
        console.error(`Error requesting ${action} for node ${nodeId}:`, error);
    }
}

function updateSidebar(node) {
    document.getElementById('node-name').textContent = node.name;

    const stateEl = document.getElementById('node-state');
    const configEl = document.getElementById('node-config');
    const latenciesEl = document.getElementById('node-latencies');

    const runBtn = document.getElementById('btn-run');
    const stopBtn = document.getElementById('btn-stop');
    const resumeBtn = document.getElementById('btn-resume');
    const finishBtn = document.getElementById('btn-finish');
    const sampleBtn = document.getElementById('btn-sample');

    if (node.type === 'binding') {
        stateEl.style.display = 'none'
        stateEl.className = 'state-indicator';

        configEl.textContent = JSON.stringify(node.config, null, 2);
        latenciesEl.textContent = 'No metrics available';

        runBtn.style.display = 'none';
        stopBtn.style.display = 'none';
        resumeBtn.style.display = 'none';
        finishBtn.style.display = 'none';
        sampleBtn.style.display = 'none';
    } else {
        const state = nodeStates[node.id] || 'Unknown';
        stateEl.style.display = 'block';
        stateEl.textContent = state;
        stateEl.className = `state-indicator ${state.toLowerCase()}`;

        configEl.textContent = JSON.stringify(node.detailedConfig || node.config, null, 2);

        if (node.latencies) {
            const metrics = node.latencies;
            let metricsText = '';

            const formatLatency = (value) => {
                if (value === null) return "null";
                if (value >= 1000000000) return `${(value / 1000000000).toFixed(5)}s`;
                if (value >= 1000000) return `${(value / 1000000).toFixed(5)}ms`;
                if (value >= 1000) return `${(value/1000).toFixed(5)}µs`;
                return `${value.toFixed(5)}ns`;
            };

            if (metrics.p50 !== null) metricsText += `P50:  ${formatLatency(metrics.p50)}\n`;
            if (metrics.p90 !== null) metricsText += `P90:  ${formatLatency(metrics.p90)}\n`;
            if (metrics.p95 !== null) metricsText += `P95:  ${formatLatency(metrics.p95)}\n`;
            if (metrics.p99 !== null) metricsText += `P99:  ${formatLatency(metrics.p99)}\n`;
            if (metrics.p100 !== null) metricsText += `P100: ${formatLatency(metrics.p100)}\n`;
            if (metrics.avg !== null) metricsText += `AVG: ${formatLatency(metrics.avg)}\n`;
            latenciesEl.textContent = metricsText.trim() ? metricsText : 'No metrics available';
        } else {
            latenciesEl.textContent = 'No metrics available';
        }

        runBtn.disabled = !['Initialized'].includes(state);
        stopBtn.disabled = !['Running', 'ResumeRequested'].includes(state);
        resumeBtn.disabled = !['StopRequested', 'Stopped'].includes(state);
        finishBtn.disabled = !['FinishRequested', 'Finished'].includes(state);
        sampleBtn.disabled = !['Running', 'ResumeRequested'].includes(state);

        runBtn.style.display = !runBtn.disabled ? 'inline-block' : 'none';
        stopBtn.style.display = !stopBtn.disabled ? 'inline-block' : 'none';
        resumeBtn.style.display = !resumeBtn.disabled ? 'inline-block' : 'none';
        finishBtn.style.display = !finishBtn.disabled ? 'inline-block' : 'none';
        sampleBtn.style.display = !sampleBtn.disabled ? 'inline-block' : 'none';

        setupButtonHandlers(node.id);
    }
}

document.querySelector('.close-btn').addEventListener('click', () => {
    document.getElementById('sidebar').classList.remove('open');
});

function startPolling() {
    setInterval(() => {
        graphData.nodes.forEach(node => {
            fetchNodeState(node.id);
        });
    }, 5000);
}

function init() {
    fetchConfig().then(() => {
        startPolling();
    });
}

init();

const postboyModal = document.getElementById('postboy-modal');
const bindingModal = document.getElementById('binding-modal');
const btnCreatePostboy = document.getElementById('btn-create-postboy');
const btnCreateBinding = document.getElementById('btn-create-binding');
const closeButtons = document.getElementsByClassName('close-modal');

btnCreatePostboy.onclick = () => {
    postboyModal.style.display = "flex";
}

btnCreateBinding.onclick = () => {
    bindingModal.style.display = "flex";
}

Array.from(closeButtons).forEach(button => {
    button.onclick = function() {
        postboyModal.style.display = "none";
        bindingModal.style.display = "none";
    }
});

window.onclick = function(event) {
    if (event.target == postboyModal) {
        postboyModal.style.display = "none";
    }
    if (event.target == bindingModal) {
        bindingModal.style.display = "none";
    }
}

document.getElementById('submit-postboy').onclick = async () => {
    const configText = document.getElementById('postboy-config').value;

    try {
        const config = JSON.parse(configText);

        const response = await fetch('/api/postboy/', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(config)
        });

        if (response.ok) {
            postboyModal.style.display = "none";
            document.getElementById('postboy-config').value = '';
            await fetchConfig();
        } else {
            alert('Failed to create Postboy: ' + await response.text());
        }
    } catch (error) {
        alert('Invalid JSON configuration: ' + error.message);
    }
}

document.getElementById('submit-binding').onclick = async () => {
    const configText = document.getElementById('binding-config').value;

    try {
        const config = JSON.parse(configText);

        const response = await fetch('/api/binding/', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(config)
        });

        if (response.ok) {
            bindingModal.style.display = "none";
            document.getElementById('binding-config').value = '';
            await fetchConfig();
        } else {
            alert('Failed to create Binding: ' + await response.text());
        }
    } catch (error) {
        alert('Invalid JSON configuration: ' + error.message);
    }
}
