package com.example.training.issue.service;

import com.example.training.common.exception.ResourceNotFoundException;
import com.example.training.issue.domain.Issue;
import com.example.training.issue.domain.IssueStatus;
import com.example.training.issue.domain.Project;
import com.example.training.issue.dto.IssueRequest;
import com.example.training.issue.dto.IssueResponse;
import com.example.training.issue.dto.IssueUpdateRequest;
import com.example.training.issue.repository.IssueRepository;
import com.example.training.issue.repository.ProjectRepository;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.transaction.annotation.Transactional;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assertions.assertThatThrownBy;

@SpringBootTest
@ActiveProfiles("test")
@Transactional
class IssueServiceTest {

    @Autowired
    private IssueService issueService;

    @Autowired
    private IssueRepository issueRepository;

    @Autowired
    private ProjectRepository projectRepository;

    private Project testProject;

    @BeforeEach
    void setUp() {
        issueRepository.deleteAll();
        projectRepository.deleteAll();
        testProject = projectRepository.save(new Project("Test Project", "Test Description"));
    }

    @Test
    void createIssue_withValidData_shouldCreateIssue() {
        IssueRequest request = new IssueRequest("Test Issue", "Test Description", null);

        IssueResponse response = issueService.createIssue(testProject.getId(), 1L, request);

        assertThat(response).isNotNull();
        assertThat(response.getTitle()).isEqualTo("Test Issue");
        assertThat(response.getStatus()).isEqualTo(IssueStatus.OPEN);
    }

    @Test
    void createIssue_withNonExistentProject_shouldThrowException() {
        IssueRequest request = new IssueRequest("Test Issue", "Test Description", null);

        assertThatThrownBy(() -> issueService.createIssue(999L, 1L, request))
                .isInstanceOf(ResourceNotFoundException.class)
                .hasMessageContaining("Project not found");
    }

    @Test
    void getIssueById_shouldIncrementViewCount() {
        Issue issue = new Issue(testProject.getId(), 1L, "Test Issue", "Description");
        issue = issueRepository.save(issue);

        IssueResponse response = issueService.getIssueById(issue.getId());

        Issue updatedIssue = issueRepository.findById(issue.getId()).orElseThrow();
        assertThat(updatedIssue.getViewCount()).isEqualTo(1);
    }

    @Test
    void updateIssue_shouldUpdateFields() {
        Issue issue = new Issue(testProject.getId(), 1L, "Original Title", "Original Description");
        issue = issueRepository.save(issue);

        IssueUpdateRequest updateRequest = new IssueUpdateRequest("Updated Title", null, IssueStatus.IN_PROGRESS, null);
        IssueResponse response = issueService.updateIssue(issue.getId(), updateRequest);

        assertThat(response.getTitle()).isEqualTo("Updated Title");
        assertThat(response.getStatus()).isEqualTo(IssueStatus.IN_PROGRESS);
    }

    @Test
    void deleteIssue_withNonExistentId_shouldThrowException() {
        assertThatThrownBy(() -> issueService.deleteIssue(999L))
                .isInstanceOf(ResourceNotFoundException.class)
                .hasMessageContaining("Issue not found");
    }
}
