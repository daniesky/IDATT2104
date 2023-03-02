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
    sendCode(code1) {
        return apiClient.post("/compile",{
            code:code1
        })
            .then((response) => {
                return response.data
            })
            .catch((error) => {
                console.log(error.response)
                return error;
            })
    }
}
