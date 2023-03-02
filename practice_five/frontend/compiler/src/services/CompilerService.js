import axios from "axios";

const apiClient = axios.create({
    baseURL: "http://localhost:8081",
    withCredentials: false,
    headers: {
        Accept: "application/json",
        "Content-Type": "application/json",
    },
});

export default {
    sendCode(code) {
        return apiClient.post("/compile",code)
            .then((response) => {
                return response.data
            })
            .catch((error) => {
                return error;
            })
    }
}
